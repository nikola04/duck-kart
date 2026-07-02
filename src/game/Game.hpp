#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/graphics/Mesh.hpp"
#include "../engine/graphics/RenderMesh.hpp"
#include <memory>

class Game final : public engine::Application {
    public:
        Game();

    protected:
        void update(float dt) override;
        void render() override;

    private:
        engine::Mesh m_pyramideMesh;
        std::unique_ptr<engine::RenderMesh> m_pyramideRenderMesh;
        engine::Transform m_pyramideTransform;
        engine::Camera m_camera;
};
