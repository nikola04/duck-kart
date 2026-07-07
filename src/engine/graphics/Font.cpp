#include "Font.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

namespace engine {
    Font::Font(const std::string& path, int size) {
        m_font = TTF_OpenFont(path.c_str(), size);

        if (!m_font)
            throw std::runtime_error("TTF_OpenFont failed");
    }

    Font::~Font() {
        if (!m_font)
            return;

        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}
