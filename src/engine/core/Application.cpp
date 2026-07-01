#include "Application.hpp"
#include <SDL3/SDL.h>

namespace engine {
    Application::Application(): m_context(), m_window(WindowSettings{
        .title = "Duck Karts",
        .width = 1280,
        .height = 720
    }), m_renderer(m_window) {}

    int Application::run() {
        auto previous_time = SDL_GetTicks();

        while(m_running) {
            SDL_Event event;

            while(SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT)
                    m_running = false;
            }

            auto current_time = SDL_GetTicks();
            float dt = static_cast<float>(current_time - previous_time) / 1000.0f;
            previous_time = current_time;

            update(dt);
            m_renderer.render();
        }

        return 0;
    }

    void Application::update(float dt) {
    }
}
