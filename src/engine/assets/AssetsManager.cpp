#include "AssetsManager.hpp"
#include <memory>

namespace engine {
    AssetsManager::AssetsManager(Renderer& renderer): m_renderer(renderer) {}

    RenderMesh* AssetsManager::createRenderMesh(const std::string& name, Mesh& mesh) {
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
}
