#include "Application.hpp"
#include <SDL3/SDL.h>
#include <iostream>

namespace engine {
    Application::Application(): m_context(), m_window(WindowSettings{
        .title = "Duck Kart",
        .width = 1280,
        .height = 720
    }), m_renderer(m_window), m_input() {}

    int Application::run() {
        auto previous_time = SDL_GetTicks();

        while(m_running) {
            SDL_Event event;
            m_input.beginFrame();

            while(SDL_PollEvent(&event)) {
                m_input.handleEvent(event);

                if (event.type == SDL_EVENT_QUIT)
                    m_running = false;

                if (event.type == SDL_EVENT_WINDOW_RESIZED)
                    m_window.setSize(event.window.data1, event.window.data2);
            }

            auto current_time = SDL_GetTicks();
            float dt = static_cast<float>(current_time - previous_time) / 1000.0f;
            previous_time = current_time;

            update(dt);

            if (!m_running)
                break;
            auto t0 = SDL_GetPerformanceCounter();

            if(!m_renderer.beginFrame()) continue;

            auto t1 = SDL_GetPerformanceCounter();

            render();

            auto t2 = SDL_GetPerformanceCounter();

            m_renderer.endFrame();

            auto t3 = SDL_GetPerformanceCounter();

            double freq = (double)SDL_GetPerformanceFrequency();

            double acquireMs = (t1 - t0) * 1000.0 / freq;
            double renderMs  = (t2 - t1) * 1000.0 / freq;
            double submitMs  = (t3 - t2) * 1000.0 / freq;

            static float timer = 0.0f;
            timer += dt;

            if (timer >= 1.0f) {
                timer = 0.0f;

                std::cout << std::fixed << std::setprecision(3)
                          << "Acquire: " << acquireMs << " ms | "
                          << "Render: "  << renderMs  << " ms | "
                          << "Submit: "  << submitMs  << " ms\n";
            }
        }

        return 0;
    }

    Renderer& Application::renderer() {
        return m_renderer;
    }

    Input& Application::input() {
        return m_input;
    }

    void Application::quit() {
        m_running = false;
    }
}
