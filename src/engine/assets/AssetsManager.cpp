#include "AssetsManager.hpp"
#include "GLTFLoader.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace engine {
    AssetsManager::AssetsManager(Renderer& renderer): m_renderer(renderer) {}

    RenderMesh* AssetsManager::createRenderMesh(const std::string& name, const Mesh& mesh) {
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

    RenderModel AssetsManager::loadModel(const std::filesystem::path& path) {
        std::string modelName = path.stem().string();
        engine::LoadedModel model;

        if (path.extension() == ".glb" || path.extension() == ".gltf")
            model = GLTFLoader::loadModel(path);
        else throw std::runtime_error(std::string("Unsupported model format on path: ") + path.generic_string());

        std::vector<const Texture*> textures;
        for (const auto& loadedTexture : model.textures) {
            auto texture = std::make_unique<engine::Texture>(m_renderer.createTexture(loadedTexture.pixels.data(), loadedTexture.width, loadedTexture.height));
            textures.push_back(texture.get());
            m_textures.push_back(std::move(texture));
        }

        RenderModel renderModel {};

        for (std::size_t i = 0; i < model.meshes.size(); ++i) {
            const auto& loadedMesh = model.meshes[i];
            engine::RenderMesh* renderMesh = this->createRenderMesh(modelName + "_mesh_" + std::to_string(i), loadedMesh.mesh);

            const engine::Texture* texture = nullptr;
            glm::vec4 baseColor{1.0f};
            float metallic = 0.0f;
            float roughness = 1.0f;

            if (loadedMesh.material >= 0) {
                const auto& material = model.materials[loadedMesh.material];
                baseColor = glm::vec4(material.baseColor, 1.0f);
                metallic = material.metallic;
                roughness = material.roughness;

                if (material.baseColorTexture >= 0) texture = textures[material.baseColorTexture];
            }

            renderModel.objects.push_back(engine::RenderObject{
                .mesh = renderMesh,
                .texture = texture,
                .baseColor = baseColor,
                .metallic = metallic,
                .roughness = roughness,
            });
        }

        return renderModel;
    }
}
