#pragma once

#include "./SDLContext.hpp"
#include "../window/Window.hpp"
#include "../graphics/Renderer.hpp"
#include "Input.hpp"

namespace engine {
    class Application {
        public:
            Application();
            virtual ~Application() = default;

            int run();

        protected:
            virtual void update(float dt) {};
            virtual void render() {};

            Renderer& renderer();
            Input& input();

        private:
            bool m_running = true;
            SDLContext m_context;
            Window m_window;
            Renderer m_renderer;
            Input m_input;
    };
}
