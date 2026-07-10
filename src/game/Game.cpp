#include "Game.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/world/components/TransformComponent.hpp"
#include "../engine/world/components/TerrainColliderComponent.hpp"
#include "../engine/world/components/ThirdPersonCameraComponent.hpp"
#include "../engine/world/components/VehicleMovementComponent.hpp"
#include "../engine/settings/EngineSettings.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <string>
#include <utility>

Game::Game(): engine::Application(), m_assets(renderer()), m_scene(), m_font("assets/fonts/geist_pixel/GeistPixel-Regular-VariableFont_ELSH.ttf", 16) {
    m_cameraEntity = m_world.createEntity();
    engine::Transform cameraTransform;
    m_world.add(m_cameraEntity, engine::TransformComponent{ .transform = cameraTransform });

    m_scene.camera.transform = cameraTransform;
    m_scene.camera.fov = 70;

    constexpr const char* trackModelPath = "assets/models/drift_race_track.glb";
    auto terrainData = m_assets.loadModelData(trackModelPath, engine::BatchStrategy::NONE);

    engine::TerrainCollider terrainCollider(engine::settings().physics.terrainCellSize);
    terrainCollider.addModel(terrainData, {});

    std::cout
        << "Terrain collider: triangles " << terrainCollider.triangleCount()
        << ", cells " << terrainCollider.cellCount()
        << ", cell size " << engine::settings().physics.terrainCellSize
        << "\n";

    engine::Transform kartTransform;
    kartTransform.scale = glm::vec3(0.04);
    kartTransform.position.z = -10.0f;
    kartTransform.position.y = 30.0f;

    m_terrainEntity = m_world.createEntity();
    m_world.add(m_terrainEntity, engine::TransformComponent{});
    m_world.add(m_terrainEntity, engine::TerrainColliderComponent{ .collider = std::move(terrainCollider) });

    m_kartEntity = m_world.createEntity();
    m_world.add(m_kartEntity, engine::TransformComponent{ .transform = kartTransform });
    m_world.add(m_kartEntity, engine::VehicleMovementComponent{});
    m_world.add(m_cameraEntity, engine::ThirdPersonCameraComponent{
        .target = m_kartEntity,
        .distance = 8.0f,
        .height = 4.0f,
        .lookAtHeight = 1.0f,
        .followSharpness = 4.0f,
    });

    m_scene.skybox.cubemap = m_assets.loadCubemap("assets/skyboxes/day");
    m_scene.addModel(m_assets.loadModel(trackModelPath, {}, engine::BatchStrategy::MATERIAL_AND_CHUNK));

    m_scene.addDynamicModel(m_kartEntity, m_assets.loadModel("assets/models/go_kart.glb", kartTransform, engine::BatchStrategy::MATERIAL));
}

void Game::update(float dt){
    m_vehicleMovementSystem.update(m_world, input(), dt);
    m_physicsSystem.snapEntityToTerrain(m_world, m_kartEntity, 1.6f, 0.0f, 100.0f);
    // m_thirdPersonCameraSystem.update(m_world, dt);

    auto* cameraTransform = m_world.get<engine::TransformComponent>(m_cameraEntity);
    if (cameraTransform)
        m_scene.camera.transform = cameraTransform->transform;

    auto* kartTransform = m_world.get<engine::TransformComponent>(m_kartEntity);
    if (kartTransform)
        m_scene.updateDynamicModelTransform(m_kartEntity, kartTransform->transform);

    m_debugHud.update(renderer(), m_font, m_scene, m_scene.camera.transform, dt);

    if (input().isKeyDown(SDL_SCANCODE_ESCAPE))
        quit();
}

void Game::render() {
    renderer().render(m_scene);
}
