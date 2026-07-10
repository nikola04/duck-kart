#include "Game.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/world/components/TransformComponent.hpp"
#include "../engine/world/components/FreeCameraControllerComponent.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>
#include <string>

Game::Game(): engine::Application(), m_assets(renderer()), m_scene(), m_font("assets/fonts/geist_pixel/GeistPixel-Regular-VariableFont_ELSH.ttf", 16), m_freeCameraSystem() {
    m_cameraEntity = m_world.createEntity();
    engine::Transform cameraTransform;
    cameraTransform.position.y = 1.0f;
    m_world.add(m_cameraEntity, engine::TransformComponent{ .transform = cameraTransform });
    m_world.add(m_cameraEntity, engine::FreeCameraControllerComponent{});

    m_scene.camera.transform = cameraTransform;
    m_scene.camera.fov = 90;

    m_scene.skybox.cubemap = m_assets.loadCubemap("assets/skyboxes/day");
    // m_scene.addModel(m_assets.loadModel("assets/models/drift_race_track.glb", {}));
    m_scene.addModel(m_assets.loadModel("assets/models/moorhuhn_kart_2_farm.glb", {}));
}

void Game::update(float dt){
    m_freeCameraSystem.update(m_world, input(), dt);

    auto* cameraTransform = m_world.get<engine::TransformComponent>(m_cameraEntity);
    if (cameraTransform)
        m_scene.camera.transform = cameraTransform->transform;

    m_debugHud.update(renderer(), m_font, m_scene, m_scene.camera.transform, dt);

    if (input().isKeyDown(SDL_SCANCODE_ESCAPE))
        quit();
}

void Game::render() {
    renderer().render(m_scene);
}
