#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/scene/RenderObject.hpp"
#include "../engine/assets/AssetsManager.hpp"
#include <vector>

class Game final : public engine::Application {
    public:
        Game();

    protected:
        void update(float dt) override;
        void render() override;

    private:
        engine::AssetsManager m_assets;
        engine::Camera m_camera;
        std::vector<engine::RenderObject> m_objects;
};
