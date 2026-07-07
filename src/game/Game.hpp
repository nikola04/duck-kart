#pragma once

#include "../engine/core/Application.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/assets/AssetsManager.hpp"
#include "../engine/graphics/Font.hpp"
#include "../engine/graphics/TextTexture.hpp"
#include <memory>

class Game final : public engine::Application {
    public:
        Game();

    protected:
        void update(float dt) override;
        void render() override;

    private:
        engine::Scene m_scene;
        engine::AssetsManager m_assets;
        engine::Font m_font;
        std::unique_ptr<engine::TextTexture> m_fpsText;
        float m_fpsTimer = 0.0f;
        int m_fpsFrameCount = 0;
};
