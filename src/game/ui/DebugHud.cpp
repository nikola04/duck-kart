#include "DebugHud.hpp"

void DebugHud::update(engine::Renderer& renderer, engine::Font& font, engine::Scene& scene, const engine::Transform& cameraTransform, float dt) {
    float fps = 1 / dt;
    m_fpsTimer += dt;
    m_fpsFrameCount++;

    if (m_fpsTimer >= 0.5f) {
        m_fpsText = std::make_unique<engine::TextTexture>(renderer, font, "FPS: " + std::to_string(static_cast<int>(fps)), SDL_Color{255, 255, 255, 200});
        const auto sceneStats = scene.stats();
        const auto renderStats = renderer.stats();
        m_sceneStatsText = std::make_unique<engine::TextTexture>(
            renderer,
            font,
            "Visible: " + std::to_string(renderStats.visibleChunks) + "/" + std::to_string(sceneStats.chunks)
                + " chunks "
                + std::to_string(renderStats.visibleObjects) + "/" + std::to_string(sceneStats.objects)
                + " objects Draws: " + std::to_string(renderStats.drawCalls),
            SDL_Color{255, 255, 255, 200}
        );

        m_fpsTimer = 0.0f;
        m_fpsFrameCount = 0;
    }

    float x = cameraTransform.position.x,
        y = cameraTransform.position.y,
        z = cameraTransform.position.z;

    m_coordsText = std::make_unique<engine::TextTexture>(renderer, font, "XYZ: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z), SDL_Color{255, 255, 255, 200});

    scene.uiTextures.clear();
    scene.uiTextures.push_back({
        .texture = &m_fpsText->texture(),
        .position = {10.0f, 10.0f},
        .size = {
            static_cast<float>(m_fpsText->width()),
            static_cast<float>(m_fpsText->height())
        },
        .color = {1, 1, 1, 1}
    });
    scene.uiTextures.push_back({
        .texture = &m_coordsText->texture(),
        .position = {10.0f, 30.0f},
        .size = {
            static_cast<float>(m_coordsText->width()),
            static_cast<float>(m_coordsText->height())
        },
        .color = {1, 1, 1, 1}
    });
    scene.uiTextures.push_back({
        .texture = &m_sceneStatsText->texture(),
        .position = {10.0f, 50.0f},
        .size = {
            static_cast<float>(m_sceneStatsText->width()),
            static_cast<float>(m_sceneStatsText->height())
        },
        .color = {1, 1, 1, 1}
    });
}
