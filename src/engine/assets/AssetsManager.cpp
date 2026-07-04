#include "AssetsManager.hpp"
#include "GLTFLoader.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

namespace engine {
    AssetsManager::AssetsManager(Renderer& renderer): m_renderer(renderer) {}

    RenderMesh* AssetsManager::createRenderMesh(const std::string& name, const Mesh& mesh) {
        if (auto* mesh = getRenderMesh(name))
            return mesh;

        auto render_mesh = std::make_unique<RenderMesh>(m_renderer.createRenderMesh(mesh));
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

        auto texture = std::make_unique<engine::Texture>(m_renderer.createTexture(loadedTexture.pixels.data(), loadedTexture.width, loadedTexture.height));
        Texture *ptr = texture.get();

        m_textures.emplace(name, std::move(texture));
        return ptr;
    }

    RenderModel AssetsManager::loadModel(const std::filesystem::path& path, Transform transform) {
        std::string modelName = path.generic_string();
        engine::LoadedModel model;

        if (path.extension() == ".glb" || path.extension() == ".gltf")
            model = GLTFLoader::loadModel(path);
        else throw std::runtime_error(std::string("Unsupported model format on path: ") + path.generic_string());

        for (std::size_t i = 0; i < model.textures.size(); i++) {
            std::string name = modelName + "#texture_" + std::to_string(i);
            createTexture(name, model.textures[i]);
        }

        RenderModel renderModel {};

        for (std::size_t i = 0; i < model.meshes.size(); ++i) {
            const auto& loadedMesh = model.meshes[i];
            engine::RenderMesh* renderMesh = createRenderMesh(modelName + "#mesh_" + std::to_string(i), loadedMesh.mesh);

            const engine::Texture* texture = nullptr;
            glm::vec4 baseColor{1.0f};
            float metallic = 0.0f;
            float roughness = 1.0f;

            if (loadedMesh.material >= 0) {
                const auto& material = model.materials[loadedMesh.material];
                baseColor = glm::vec4(material.baseColor, 1.0f);
                metallic = material.metallic;
                roughness = material.roughness;

                if (material.baseColorTexture >= 0) texture = getTexture(modelName + "#texture_" + std::to_string(material.baseColorTexture));
            }

            renderModel.objects.push_back(engine::RenderObject{
                .mesh = renderMesh,
                .texture = texture,
                .baseColor = baseColor,
                .metallic = metallic,
                .roughness = roughness,
                .transform = transform
            });
        }

        return renderModel;
    }
}
