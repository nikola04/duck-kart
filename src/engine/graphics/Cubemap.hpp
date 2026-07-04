#pragma once

#include "GPUTexture.hpp"
#include <cstdint>

namespace engine {
    class Cubemap : public GPUTexture {
        public:
            Cubemap(SDL_GPUDevice* device, SDL_GPUTexture* texture, std::uint32_t size): GPUTexture(device, texture), m_size(size) {}

            uint32_t size() const {
                return m_size;
            }

        private:
            std::uint32_t m_size;
    };
}
