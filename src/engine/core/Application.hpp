#pragma once

#include "../window/Window.hpp"
#include "./SDLContext.hpp"

namespace engine {
    class Application {
        public:
            Application();
            virtual ~Application() = default;

            int run();

        protected:
            virtual void update(float dt);

        private:
            bool m_running = true;
            SDLContext m_context;
            Window m_window;
    };
}
