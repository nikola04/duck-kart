#pragma once

#include <SDL3/SDL.h>

namespace engine {
    class Input {
        public:
            void handleEvent(const SDL_Event& event);

            bool isKeyDown(const SDL_Scancode key) const;
            bool isPressed(const SDL_Scancode key) const;

            float mouseDeltaX() const;
            float mouseDeltaY() const;

            void beginFrame();

        private:
            const bool* m_keyboard_state = nullptr;
            const bool* m_pressed_event = nullptr;

            float m_mouse_dx, m_mouse_dy;
    };
}
