#include "TextTexture.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

namespace engine {
    TextTexture::TextTexture(Renderer& renderer, Font& font, const std::string& text, SDL_Color color): m_texture(nullptr, nullptr, 0, 0) {
        SDL_Surface* surface = TTF_RenderText_Blended(font.handle(), text.c_str(), 0, color);
        if (!surface)
            throw std::runtime_error("TTF_RenderText_Blended failed");

        SDL_Surface* rgba = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);

        if (!rgba)
            throw std::runtime_error("SDL_ConvertSurface failed");

        m_width = rgba->w;
        m_height = rgba->h;

        m_texture = renderer.resources().createTexture(rgba->pixels, m_width, m_height, false);
        SDL_DestroySurface(rgba);
    }
}
