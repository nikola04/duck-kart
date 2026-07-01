#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace engine {

    struct WindowSettings {
        std::string title = "Kart Engine";
        int width = 1280;
        int height = 720;

        bool borderless = false;
        bool resizable = true;
    };

    class Window {
        public:
            Window(const WindowSettings& settings);
            ~Window();

            // delete on copy
            Window(const Window&) = delete;
            Window& operator = (const Window&) = delete;

            SDL_Window* nativeHandle() const;

            int width() const;
            int height() const;

        private:
            SDL_Window* m_window = nullptr;

            int m_width;
            int m_height;
    };
}
