#pragma once

#include <SDL3/SDL_gpu.h>
#include <cstdint>

namespace engine {
    class ShadowMap {
        public:
            ShadowMap(SDL_GPUDevice* device, uint32_t width, uint32_t height);
            ~ShadowMap();

            ShadowMap(const ShadowMap&) = delete;
            ShadowMap& operator=(const ShadowMap&) = delete;
            ShadowMap(ShadowMap&& other) noexcept;
            ShadowMap& operator=(ShadowMap&& other) noexcept;

            SDL_GPUTexture* texture() const;

            std::uint32_t width() const;
            std::uint32_t height() const;

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUTexture* m_texture = nullptr;

            std::uint32_t m_width;
            std::uint32_t m_height;
        };
}
