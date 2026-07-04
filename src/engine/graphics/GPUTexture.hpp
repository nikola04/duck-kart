#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>

namespace engine {
    class GPUTexture {
        public:
            GPUTexture(SDL_GPUDevice* device, SDL_GPUTexture* texture);
            virtual ~GPUTexture();

            GPUTexture(const GPUTexture&) = delete;
            GPUTexture& operator = (const GPUTexture&) = delete;
            GPUTexture(GPUTexture&& other) noexcept;
            GPUTexture& operator=(GPUTexture&& other) noexcept;

            SDL_GPUTexture* handle() const;
        protected:
            SDL_GPUDevice* m_device;
            SDL_GPUTexture* m_texture;
    };
}
