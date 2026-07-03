#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>

namespace engine {
    class Texture {
        public:
            Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture, Uint32 width, Uint32 height);
            ~Texture();

            Texture(const Texture&) = delete;
            Texture& operator = (const Texture&) = delete;
            Texture(Texture&& other) noexcept;
            Texture& operator=(Texture&& other) noexcept;

            SDL_GPUTexture* handle() const;

            Uint32 width() const;
            Uint32 height() const;

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUTexture* m_texture;

            Uint32 m_width = 0;
            Uint32 m_height = 0;
    };
}
