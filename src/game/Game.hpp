#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/assets/AssetsManager.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/world/World.hpp"
#include "../engine/world/systems/FreeCameraControllerSystem.hpp"
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

        // ----   systems   ----
        engine::FreeCameraControllerSystem m_freeCameraSystem;
        // ---- END Systems ----

        engine::Font m_font;
        DebugHud m_debugHud;
};
