#pragma once

#include <SDL3/SDL.h>
#include "../window/Window.hpp"

namespace engine {
    class Renderer {
        public:
            Renderer(Window& window);
            ~Renderer();

            Renderer(const Renderer& renderer) = delete;
            Renderer& operator = (const Renderer& renderer) = delete;

            void render();

        private:
            Window& m_window;
            SDL_GPUDevice* m_device = nullptr;
    };
}
