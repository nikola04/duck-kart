#pragma once

#include "Font.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include <SDL3/SDL_pixels.h>
#include <string>

namespace engine {
    class TextTexture {
        public:
            TextTexture(Renderer& renderer, Font& font, const std::string& text, SDL_Color color);

            const Texture& texture() const { return m_texture; };
            int width() const { return m_width; };
            int height() const { return m_height; };

        private:
            Texture m_texture;
            int m_width;
            int m_height;
    };
}
