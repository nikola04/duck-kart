#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

namespace engine {
    class RenderMesh {
    public:
        RenderMesh(
            SDL_GPUDevice* device,
            SDL_GPUBuffer* vertexBuffer,
            SDL_GPUBuffer* indexBuffer,
            std::uint32_t indexCount
        );

        ~RenderMesh();

        RenderMesh(const RenderMesh&) = delete;
        RenderMesh& operator=(const RenderMesh&) = delete;

        RenderMesh(RenderMesh&& other) noexcept;
        RenderMesh& operator=(RenderMesh&& other) noexcept;

        SDL_GPUBuffer* vertexBuffer() const;
        SDL_GPUBuffer* indexBuffer() const;
        std::uint32_t indexCount() const;

    private:
        SDL_GPUDevice* m_device = nullptr;
        SDL_GPUBuffer* m_vertexBuffer = nullptr;
        SDL_GPUBuffer* m_indexBuffer = nullptr;
        std::uint32_t m_indexCount = 0;
    };
}
