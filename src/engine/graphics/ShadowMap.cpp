#include "ShadowMap.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <stdexcept>
#include <string>

namespace engine {
    ShadowMap::ShadowMap(SDL_GPUDevice* device, uint32_t width, uint32_t height): m_device(device), m_width(width), m_height(height) {
        SDL_GPUTextureCreateInfo info{};
        info.type = SDL_GPU_TEXTURETYPE_2D;
        info.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
        info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
        info.width = width;
        info.height = height;
        info.layer_count_or_depth = 1;
        info.num_levels = 1;
        info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        m_texture = SDL_CreateGPUTexture(m_device, &info);
        if (!m_texture)
            throw std::runtime_error(std::string("SDL_CreateGPUTexture in ShadowMap failed: ") + SDL_GetError());
    }

    ShadowMap::~ShadowMap() {
        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);
    }

    ShadowMap::ShadowMap(ShadowMap&& other) noexcept : m_device(other.m_device), m_texture(other.m_texture), m_width(other.m_width), m_height(other.m_height) {
        other.m_device = nullptr;
        other.m_texture = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }

    ShadowMap& ShadowMap::operator=(ShadowMap&& other) noexcept {
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

    SDL_GPUTexture* ShadowMap::texture() const {
        return m_texture;
    }

    std::uint32_t ShadowMap::width() const {
        return m_width;
    }

    std::uint32_t ShadowMap::height() const {
        return m_height;
    }
}
