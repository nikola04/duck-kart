#pragma once

#include "../graphics/Renderer.hpp"
#include "LoadedModel.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace engine {
    class AssetsManager {
        public:
            AssetsManager(Renderer& renderer);

            RenderModel loadModel(const std::filesystem::path& path, Transform transform);
            const Cubemap* loadCubemap(const std::filesystem::path& path);

            RenderMesh* getRenderMesh(const std::string& name);
            RenderMesh* createRenderMesh(const std::string& name, const Mesh& mesh);

            Texture* getTexture(const std::string& name);
            Texture* createTexture(const std::string& name, const LoadedTexture& loadedTexture);

            Material* getMaterial(const std::string& name);
            Material* createMaterial(const std::string& name, const LoadedMaterial& loadedMaterial, const Texture* texture, const Texture* normalTexture);

        private:
            Renderer& m_renderer;

            Material m_default_material;

            std::unordered_map<std::string, std::unique_ptr<engine::Texture>> m_textures;
            std::unordered_map<std::string, std::unique_ptr<engine::Material>> m_materials;
            std::unordered_map<std::string, std::unique_ptr<RenderMesh>> m_render_meshes;
            std::unordered_map<std::string, std::unique_ptr<Cubemap>> m_cubemaps;
    };
}
