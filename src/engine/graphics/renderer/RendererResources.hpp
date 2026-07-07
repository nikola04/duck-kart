#pragma once

#include "../Mesh.hpp"
#include "../Texture.hpp"
#include "../Cubemap.hpp"
#include "RenderMesh.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <array>
#include <vector>

namespace engine {
    class RendererResources {
        public:
            RendererResources(SDL_GPUDevice* device): m_device(device) {}
            ~RendererResources() {
                if (m_depth_texture)
                    SDL_ReleaseGPUTexture(m_device, m_depth_texture);
            }

            void createDepthTexture(Uint32 width, Uint32 height);
            void ensureDepthTexture(Uint32 width, Uint32 height);
            SDL_GPUTexture* depthTexture() const;
            Texture createTexture(const void* pixels, Uint32 width, Uint32 height, bool generate_mipmaps = true);

            RenderMesh createRenderMesh(const Mesh& mesh);
            Cubemap createCubemap(std::array<std::vector<std::uint8_t>, 6>& faces, std::uint32_t size = 0);

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUTexture* m_depth_texture = nullptr;
            Uint32 m_depth_width = 0;
            Uint32 m_depth_height = 0;
    };
}
