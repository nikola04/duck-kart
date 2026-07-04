#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/assets/AssetsManager.hpp"

class Game final : public engine::Application {
    public:
        Game();

    protected:
        void update(float dt) override;
        void render() override;

    private:
        engine::Scene m_scene;
        engine::AssetsManager m_assets;
};
