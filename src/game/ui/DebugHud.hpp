#pragma once

#include "../../engine/graphics/Renderer.hpp"
#include "../../engine/graphics/Font.hpp"
#include "../../engine/graphics/TextTexture.hpp"

class DebugHud {
    public:
        void update(engine::Renderer& renderer, engine::Font& font, engine::Scene& scene, const engine::Transform& cameraTransform, float dt);

    private:
        float m_fpsTimer = 1.0f;
        int m_fpsFrameCount = 0;

        std::unique_ptr<engine::TextTexture> m_fpsText;
        std::unique_ptr<engine::TextTexture> m_coordsText;
        std::unique_ptr<engine::TextTexture> m_sceneStatsText;
};
