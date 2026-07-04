#include "Texture.hpp"

namespace engine {
    Texture::Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture, Uint32 width, Uint32 height): GPUTexture(device, texture), m_width(width), m_height(height) {}

    Uint32 Texture::width() const {
        return m_width;
    }

    Uint32 Texture::height() const {
        return m_height;
    }
}
