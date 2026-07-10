#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/assets/AssetsManager.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/world/World.hpp"
#include "../engine/world/systems/PhysicsSystem.hpp"
#include "../engine/world/systems/ThirdPersonCameraSystem.hpp"
#include "../engine/world/systems/VehicleMovementSystem.hpp"
#include "ui/DebugHud.hpp"

class Game final : public engine::Application {
    public:
        Game();

    protected:
        void update(float dt) override;
        void render() override;

    private:
        engine::Scene m_scene;
        engine::AssetsManager m_assets;
        engine::World m_world;
        engine::Entity m_cameraEntity = engine::NullEntity;
        engine::Entity m_terrainEntity = engine::NullEntity;
        engine::Entity m_kartEntity = engine::NullEntity;

        // ----   systems   ----
        engine::PhysicsSystem m_physicsSystem;
        engine::VehicleMovementSystem m_vehicleMovementSystem;
        engine::ThirdPersonCameraSystem m_thirdPersonCameraSystem;
        // ---- END Systems ----

        engine::Font m_font;
        DebugHud m_debugHud;
};
