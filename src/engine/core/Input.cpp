#include "Input.hpp"

namespace engine {
    void Input::handleEvent(const SDL_Event& event) {
        m_keyboard_state = SDL_GetKeyboardState(nullptr);

        if (event.type == SDL_EVENT_MOUSE_MOTION){
            m_mouse_dx += event.motion.xrel;
            m_mouse_dy += event.motion.yrel;
        }
    }

    void Input::beginFrame() {
        m_mouse_dx = 0.0f;
        m_mouse_dy = 0.0f;
    }

    bool Input::isKeyDown(SDL_Scancode key) const {
        if (!m_keyboard_state) return false;

        return m_keyboard_state[key];
    }

    float Input::mouseDeltaX() const {
        return m_mouse_dx;
    }

    float Input::mouseDeltaY() const {
        return m_mouse_dy;
    }
}
