#include "RenderMesh.hpp"

namespace engine {
    RenderMesh::RenderMesh(
        SDL_GPUDevice* device,
        SDL_GPUBuffer* vertexBuffer,
        SDL_GPUBuffer* indexBuffer,
        std::uint32_t indexCount
    )
        : m_device(device),
          m_vertexBuffer(vertexBuffer),
          m_indexBuffer(indexBuffer),
          m_indexCount(indexCount)
    {
    }

    RenderMesh::~RenderMesh() {
        if (m_vertexBuffer)
            SDL_ReleaseGPUBuffer(m_device, m_vertexBuffer);

        if (m_indexBuffer)
            SDL_ReleaseGPUBuffer(m_device, m_indexBuffer);
    }

    RenderMesh::RenderMesh(RenderMesh&& other) noexcept
        : m_device(other.m_device),
          m_vertexBuffer(other.m_vertexBuffer),
          m_indexBuffer(other.m_indexBuffer),
          m_indexCount(other.m_indexCount)
    {
        other.m_device = nullptr;
        other.m_vertexBuffer = nullptr;
        other.m_indexBuffer = nullptr;
        other.m_indexCount = 0;
    }

    RenderMesh& RenderMesh::operator=(RenderMesh&& other) noexcept {
        if (this == &other)
            return *this;

        if (m_vertexBuffer)
            SDL_ReleaseGPUBuffer(m_device, m_vertexBuffer);

        if (m_indexBuffer)
            SDL_ReleaseGPUBuffer(m_device, m_indexBuffer);

        m_device = other.m_device;
        m_vertexBuffer = other.m_vertexBuffer;
        m_indexBuffer = other.m_indexBuffer;
        m_indexCount = other.m_indexCount;

        other.m_device = nullptr;
        other.m_vertexBuffer = nullptr;
        other.m_indexBuffer = nullptr;
        other.m_indexCount = 0;

        return *this;
    }

    SDL_GPUBuffer* RenderMesh::vertexBuffer() const {
        return m_vertexBuffer;
    }

    SDL_GPUBuffer* RenderMesh::indexBuffer() const {
        return m_indexBuffer;
    }

    std::uint32_t RenderMesh::indexCount() const {
        return m_indexCount;
    }
}
