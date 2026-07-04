#include "GPUTexture.hpp"

namespace engine {
    GPUTexture::GPUTexture(SDL_GPUDevice* device, SDL_GPUTexture* texture): m_device(device), m_texture(texture){}

    GPUTexture::~GPUTexture(){
        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);
    }

    GPUTexture::GPUTexture(GPUTexture&& other) noexcept : m_device(other.m_device), m_texture(other.m_texture) {
        other.m_device = nullptr;
        other.m_texture = nullptr;
    }

    GPUTexture& GPUTexture::operator=(GPUTexture&& other) noexcept {
        if (this == &other)
            return *this;

        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);

        m_device = other.m_device;
        m_texture = other.m_texture;

        other.m_device = nullptr;
        other.m_texture = nullptr;

        return *this;
    }

    SDL_GPUTexture* GPUTexture::handle() const {
        return m_texture;
    }
}
