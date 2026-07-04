#pragma once

#include "GPUTexture.hpp"

namespace engine {
    class Texture : public GPUTexture {
        public:
            Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture, Uint32 width, Uint32 height);

            Uint32 width() const;
            Uint32 height() const;

        private:
            Uint32 m_width = 0;
            Uint32 m_height = 0;
    };
}
