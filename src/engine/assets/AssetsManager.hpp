#pragma once

#include "../graphics/Renderer.hpp"
#include "GLTFLoader.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {
    struct RenderModel {
        std::vector<RenderObject> objects;
    };

    class AssetsManager {
        public:
            AssetsManager(Renderer& renderer);

            RenderModel loadModel(const std::filesystem::path& path, Transform transform);

            RenderMesh* getRenderMesh(const std::string& name);
            RenderMesh* createRenderMesh(const std::string& name, const Mesh& mesh);

            Texture* getTexture(const std::string& name);
            Texture* createTexture(const std::string& name, const LoadedTexture& loadedTexture);

        private:
            Renderer& m_renderer;

            std::unordered_map<std::string, std::unique_ptr<engine::Texture>> m_textures;
            std::unordered_map<std::string, std::unique_ptr<RenderMesh>> m_render_meshes;
    };
}
