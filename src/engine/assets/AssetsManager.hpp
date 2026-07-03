#pragma once

#include "../graphics/Renderer.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {
    class AssetsManager {
        public:
            AssetsManager(Renderer& renderer);

            RenderMesh* createRenderMesh(const std::string& name, Mesh& mesh);
            RenderMesh* getRenderMesh(const std::string& name);

        private:
            Renderer& m_renderer;

            std::unordered_map<std::string, std::unique_ptr<RenderMesh>> m_render_meshes;
    };
}
