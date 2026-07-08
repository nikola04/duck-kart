#include "AssetsManager.hpp"
#include "GLTFLoader.hpp"
#include "LoadedModelBatcher.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <stb_image.h>

namespace engine {
    AssetsManager::AssetsManager(Renderer& renderer): m_renderer(renderer), m_default_material() {}

    RenderMesh* AssetsManager::createRenderMesh(const std::string& name, const Mesh& mesh) {
        if (auto* mesh = getRenderMesh(name))
            return mesh;

        auto render_mesh = std::make_unique<RenderMesh>(m_renderer.resources().createRenderMesh(mesh));
        RenderMesh* ptr = render_mesh.get();

        m_render_meshes.emplace(name, std::move(render_mesh));
        return ptr;
    }

    RenderMesh* AssetsManager::getRenderMesh(const std::string& name) {
        auto it = m_render_meshes.find(name);
        if (it == m_render_meshes.end()) return nullptr;

        return it->second.get();
    }

    Texture* AssetsManager::getTexture(const std::string& name) {
        auto it = m_textures.find(name);
        if (it == m_textures.end()) return nullptr;

        return it->second.get();
    }

    Texture* AssetsManager::createTexture(const std::string& name, const LoadedTexture& loadedTexture) {
        if (auto* texture = getTexture(name))
            return texture;

        auto texture = std::make_unique<engine::Texture>(m_renderer.resources().createTexture(loadedTexture.pixels.data(), loadedTexture.width, loadedTexture.height));
        Texture *ptr = texture.get();

        m_textures.emplace(name, std::move(texture));
        return ptr;
    }

    Material* AssetsManager::getMaterial(const std::string& name) {
        auto it = m_materials.find(name);
        if (it == m_materials.end()) return nullptr;

        return it->second.get();
    }

    Material* AssetsManager::createMaterial(const std::string& name, const LoadedMaterial& loadedMaterial, const Texture* texture, const Texture* normalTexture) {
        if (auto* material = getMaterial(name))
            return material;

        auto material = std::make_unique<Material> (glm::vec4(loadedMaterial.baseColor, 1.0f), texture, normalTexture, loadedMaterial.metallic, loadedMaterial.roughness, loadedMaterial.alphaMode, loadedMaterial.alphaCutoff);
        Material* ptr = material.get();

        m_materials.emplace(name, std::move(material));
        return ptr;
    }

    RenderModel AssetsManager::loadModel(const std::filesystem::path& path, Transform transform) {
        std::string modelName = path.generic_string();
        engine::LoadedModel model;

        if (path.extension() == ".glb" || path.extension() == ".gltf") {
            GLTFLoader loader;
            model = LoadedModelBatcher::batchByMaterialAndChunk(loader.loadModel(path), 64);
        } else throw std::runtime_error(std::string("Unsupported model format on path: ") + path.generic_string());

        for (std::size_t i = 0; i < model.textures.size(); i++) {
            std::string name = modelName + "#texture_" + std::to_string(i);
            createTexture(name, model.textures[i]);
        }

        RenderModel renderModel {};

        for (std::size_t i = 0; i < model.meshes.size(); ++i) {
            const auto& loadedMesh = model.meshes[i];
            engine::RenderMesh* renderMesh = createRenderMesh(modelName + "#mesh_" + std::to_string(i), loadedMesh.mesh);

            Material* material = &m_default_material;
            if (loadedMesh.material >= 0) {
                const auto& loadedMaterial = model.materials[loadedMesh.material];
                const Texture* texture = nullptr;
                const Texture* normalTexture = nullptr;

                if (loadedMaterial.baseColorTexture >= 0) texture = getTexture(modelName + "#texture_" + std::to_string(loadedMaterial.baseColorTexture));
                if (loadedMaterial.normalTexture >= 0) normalTexture = getTexture(modelName + "#texture_" + std::to_string(loadedMaterial.normalTexture));

                material = createMaterial(modelName + "#material_" + std::to_string(loadedMesh.material), loadedMaterial, texture, normalTexture);
            }

            renderModel.objects.push_back(engine::RenderObject{
                .mesh = renderMesh,
                .material = material,
                .transform = transform,
            });
        }

        return renderModel;
    }

    const Cubemap* AssetsManager::loadCubemap(const std::filesystem::path& folder) {
        auto it = m_cubemaps.find(folder);
        if (it != m_cubemaps.end()) return it->second.get();

        std::array<std::filesystem::path, 6> paths{
            folder / "px.png",
            folder / "nx.png",
            folder / "py.png",
            folder / "ny.png",
            folder / "pz.png",
            folder / "nz.png"
        };
        std::array<std::vector<std::uint8_t>, 6> faces;
        int size = 0;

        for (std::size_t i = 0; i < 6; i++) {
            int w, h, c;
            stbi_uc* data = stbi_load(paths[i].string().c_str(), &w, &h, &c, 4);

            if (!data)
                throw std::runtime_error("Failed to load cubemap face: " + paths[i].string());

            if (w != h)
                throw std::runtime_error("Cubemap face must be square: " + paths[i].string());

            if (i == 0)
                size = w;
            else if (w != size || h != size)
                throw std::runtime_error("All cubemap faces must have same size");

            faces[i] = std::vector<std::uint8_t>(data, data + w * h * 4);
            stbi_image_free(data);
        }

        auto cubemap = std::make_unique<Cubemap>(m_renderer.resources().createCubemap(faces, static_cast<uint32_t>(size)));
        const Cubemap* ptr = cubemap.get();

        m_cubemaps.emplace(folder.generic_string(), std::move(cubemap));
        return ptr;
    }
}
