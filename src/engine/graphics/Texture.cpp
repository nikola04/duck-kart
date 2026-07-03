#include "Texture.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

namespace engine {
    Texture::Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture, Uint32 width, Uint32 height): m_device(device), m_texture(texture), m_width(width), m_height(height) {}
    Texture::~Texture() {
        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);
        m_texture = nullptr;
    }

    Texture::Texture(Texture&& other) noexcept : m_device(other.m_device), m_texture(other.m_texture), m_width(other.m_width), m_height(other.m_height) {
         other.m_device = nullptr;
         other.m_texture = nullptr;
         other.m_width = 0;
         other.m_height = 0;
     }

     Texture& Texture::operator=(Texture&& other) noexcept {
         if (this == &other)
             return *this;

         if (m_texture)
             SDL_ReleaseGPUTexture(m_device, m_texture);

         m_device = other.m_device;
         m_texture = other.m_texture;
         m_width = other.m_width;
         m_height = other.m_height;
         other.m_device = nullptr;
         other.m_texture = nullptr;
         other.m_width = 0;
         other.m_height = 0;

         return *this;
     }

    SDL_GPUTexture* Texture::handle() const {
        return m_texture;
    }

    Uint32 Texture::width() const {
        return m_width;
    }

    Uint32 Texture::height() const {
        return m_height;
    }
}
