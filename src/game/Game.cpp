#include "Game.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/graphics/TextTexture.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>
#include <string>

Game::Game(): engine::Application(), m_assets(renderer()), m_scene(), m_font("assets/fonts/geist_pixel/GeistPixel-Regular-VariableFont_ELSH.ttf", 16){
    // engine::PointLight light{
    //     .position = {0.0f, 5.0f, 5.0f, 1.0f},
    //     .color = {1.0f, 0.9f, 0.7f, 1.0f},
    //     .properties = {5.0f, 15.0f, 0.0f, 0.0f}
    // };
    // m_scene.pointLights.push_back(light);

    // engine::Transform bulbTransform;
    // bulbTransform.position = glm::vec3(light.position);
    // bulbTransform.scale = {0.2f, 0.2f, 0.2f};
    // m_scene.addModel(m_assets.loadModel("assets/models/sphere.glb", bulbTransform));

    // engine::Transform transform{};
    // transform.scale = { 50.0f, 50.0f, 50.0f};
    // m_scene.addModel(m_assets.loadModel("assets/models/sample.glb", transform));

    m_scene.camera.fov = 90;
    m_scene.camera.transform.position.y = 1.0f;
    m_scene.skybox.cubemap = m_assets.loadCubemap("assets/skyboxes/day");
    m_scene.addModel(m_assets.loadModel("assets/models/drift_race_track.glb", {}));
}

void Game::update(float dt){
    float speed = 3.0f;
    float fps = 1 / dt;
    const float mouseSensitivity = 0.0025f;
    const float maxPitch = glm::radians(89.0f);

    m_fpsTimer += dt;
    m_fpsFrameCount++;

    if (m_fpsTimer >= 0.25f) {
        m_fpsText = std::make_unique<engine::TextTexture>(renderer(), m_font, "FPS: " + std::to_string(static_cast<int>(fps)), SDL_Color{255, 255, 255, 200});
        const auto sceneStats = m_scene.stats();
        const auto renderStats = renderer().stats();
        m_sceneStatsText = std::make_unique<engine::TextTexture>(
            renderer(),
            m_font,
            "Visible: " + std::to_string(renderStats.visibleChunks) + "/" + std::to_string(sceneStats.chunks)
                + " chunks "
                + std::to_string(renderStats.visibleObjects) + "/" + std::to_string(sceneStats.objects)
                + " objects Draws: " + std::to_string(renderStats.drawCalls),
            SDL_Color{255, 255, 255, 200}
        );

        m_fpsTimer = 0.0f;
        m_fpsFrameCount = 0;
    }

    float x = m_scene.camera.transform.position.x,
        y = m_scene.camera.transform.position.y,
        z = m_scene.camera.transform.position.z;

    m_coordsText = std::make_unique<engine::TextTexture>(renderer(), m_font, "XYZ: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z), SDL_Color{255, 255, 255, 200});

    m_scene.uiTextures.clear();
    if (m_debugEnabled) {
        m_scene.uiTextures.push_back({
            .texture = &m_fpsText->texture(),
            .position = {10.0f, 10.0f},
            .size = {
                static_cast<float>(m_fpsText->width()),
                static_cast<float>(m_fpsText->height())
            },
            .color = {1, 1, 1, 1}
        });
        m_scene.uiTextures.push_back({
            .texture = &m_coordsText->texture(),
            .position = {10.0f, 30.0f},
            .size = {
                static_cast<float>(m_coordsText->width()),
                static_cast<float>(m_coordsText->height())
            },
            .color = {1, 1, 1, 1}
        });
        m_scene.uiTextures.push_back({
            .texture = &m_sceneStatsText->texture(),
            .position = {10.0f, 50.0f},
            .size = {
                static_cast<float>(m_sceneStatsText->width()),
                static_cast<float>(m_sceneStatsText->height())
            },
            .color = {1, 1, 1, 1}
        });
    }

    // mouse movement
    m_scene.camera.transform.rotation.y += input().mouseDeltaX() * mouseSensitivity;
    m_scene.camera.transform.rotation.x -= input().mouseDeltaY() * mouseSensitivity;
    m_scene.camera.transform.rotation.x = std::clamp(m_scene.camera.transform.rotation.x, -maxPitch, maxPitch);

    if (input().isKeyDown(SDL_SCANCODE_LCTRL))
        speed = 30.0f;
    // keyboard movement
    if (input().isKeyDown(SDL_SCANCODE_W))
        m_scene.camera.transform.position += m_scene.camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_S))
        m_scene.camera.transform.position -= m_scene.camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_A))
        m_scene.camera.transform.position -= m_scene.camera.transform.right() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_D))
        m_scene.camera.transform.position += m_scene.camera.transform.right() * speed * dt;

    if (input().isKeyDown(SDL_SCANCODE_ESCAPE))
        quit();
}

void Game::render() {
    renderer().render(m_scene);
}
