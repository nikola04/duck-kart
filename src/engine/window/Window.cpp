#include "Window.hpp"

namespace engine {
    Window::Window(const WindowSettings& settings): m_width(settings.width), m_height(settings.height) {
        SDL_WindowFlags flags = 0;

        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (settings.resizable)
            flags |= SDL_WINDOW_RESIZABLE;
        if (settings.borderless)
            flags |= SDL_WINDOW_BORDERLESS;

        m_window = SDL_CreateWindow(settings.title.c_str(), settings.width, settings.height, flags);

        if (!m_window)
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

        setRelativeMouseMode(true);
    }

    Window::~Window() {
        SDL_DestroyWindow(m_window);
    }

    SDL_Window* Window::handle() const {
        return m_window;
    }

    int Window::width() const {
        return m_width;
    }

    int Window::height() const {
        return m_height;
    }

    void Window::setSize(int width, int height) {
        m_width = width;
        m_height = height;
    }

    void Window::setRelativeMouseMode(bool enabled) {
        SDL_SetWindowRelativeMouseMode(m_window, enabled);
    }
}
