#include "Game.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/graphics/TextTexture.hpp"
#include "../engine/world/components/TransformComponent.hpp"
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
    // transform.scale = { 5.0f, 5.0f, 5.0f};
    // transform.position.y = 1.0f;
    // transform.position.x = 10.0f;
    // m_scene.addModel(m_assets.loadModel("assets/models/mr._pumpkin.glb", transform));

    m_cameraEntity = m_world.createEntity();
    engine::Transform cameraTransform;
    cameraTransform.position.y = 1.0f;
    m_world.add(m_cameraEntity, engine::TransformComponent{ .transform = cameraTransform });
    m_scene.camera.transform = cameraTransform;
    m_scene.camera.fov = 90;

    m_scene.skybox.cubemap = m_assets.loadCubemap("assets/skyboxes/day");
    m_scene.addModel(m_assets.loadModel("assets/models/drift_race_track.glb", {}));
    // m_scene.addModel(m_assets.loadModel("assets/models/moorhuhn_kart_2_farm.glb", {}));
}

void Game::update(float dt){
    float speed = 3.0f;
    float fps = 1 / dt;
    const float mouseSensitivity = 0.0025f;
    const float maxPitch = glm::radians(89.0f);
    auto* cameraTransform = m_world.get<engine::TransformComponent>(m_cameraEntity);
    if (!cameraTransform)
        return;

    m_fpsTimer += dt;
    m_fpsFrameCount++;

    if (m_fpsTimer >= 0.5f) {
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

    float x = cameraTransform->transform.position.x,
        y = cameraTransform->transform.position.y,
        z = cameraTransform->transform.position.z;

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
    cameraTransform->transform.rotation.y += input().mouseDeltaX() * mouseSensitivity;
    cameraTransform->transform.rotation.x -= input().mouseDeltaY() * mouseSensitivity;
    cameraTransform->transform.rotation.x = std::clamp(cameraTransform->transform.rotation.x, -maxPitch, maxPitch);

    if (input().isKeyDown(SDL_SCANCODE_LCTRL))
        speed = 30.0f;
    // keyboard movement
    if (input().isKeyDown(SDL_SCANCODE_W))
        cameraTransform->transform.position += cameraTransform->transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_S))
        cameraTransform->transform.position -= cameraTransform->transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_A))
        cameraTransform->transform.position -= cameraTransform->transform.right() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_D))
        cameraTransform->transform.position += cameraTransform->transform.right() * speed * dt;

    m_scene.camera.transform = cameraTransform->transform;

    if (input().isKeyDown(SDL_SCANCODE_ESCAPE))
        quit();
}

void Game::render() {
    renderer().render(m_scene);
}
