#pragma once

#include "../graphics/Renderer.hpp"
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

            RenderMesh* createRenderMesh(const std::string& name, const Mesh& mesh);
            RenderMesh* getRenderMesh(const std::string& name);
            RenderModel loadModel(const std::filesystem::path& path);

        private:
            Renderer& m_renderer;

            std::vector<std::unique_ptr<engine::Texture>> m_textures;
            std::unordered_map<std::string, std::unique_ptr<RenderMesh>> m_render_meshes;
    };
}
