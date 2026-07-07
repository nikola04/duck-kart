#include "RendererResources.hpp"
#include "../../math/Math.hpp"
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace engine {
    void RendererResources::createDepthTexture(Uint32 width, Uint32 height) {
        if (m_depth_texture) {
            SDL_ReleaseGPUTexture(m_device, m_depth_texture);
            m_depth_texture = nullptr;
        }

        SDL_GPUTextureCreateInfo info{};
        info.type = SDL_GPU_TEXTURETYPE_2D;
        info.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
        info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        info.width = width;
        info.height = height;
        info.layer_count_or_depth = 1;
        info.num_levels = 1;
        info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        m_depth_texture = SDL_CreateGPUTexture(m_device, &info);
        if (!m_depth_texture)
            throw std::runtime_error(std::string("SDL_CreateGPUTexture depth failed: ") + SDL_GetError());

        m_depth_width = width;
        m_depth_height = height;
    }

    void RendererResources::ensureDepthTexture(Uint32 width, Uint32 height) {
        if (!m_depth_texture || width != m_depth_width || height != m_depth_height)
            createDepthTexture(width, height);
    }

    SDL_GPUTexture* RendererResources::depthTexture() const {
        return m_depth_texture;
    }

    Texture RendererResources::createTexture(const void* pixels, Uint32 width, Uint32 height, bool generate_mipmaps) {
        if (!pixels)
            throw std::invalid_argument("Cannot create a texture from null pixel data");

        if (width == 0 || height == 0)
            throw std::invalid_argument("Cannot create a texture with zero dimensions");

        constexpr Uint32 bytes_per_pixel = 4;
        const std::uint64_t upload_size_64 = static_cast<std::uint64_t>(width) * height * bytes_per_pixel;
        if (upload_size_64 > std::numeric_limits<Uint32>::max())
            throw std::length_error("Texture is too large for an SDL GPU transfer buffer");

        const Uint32 upload_size = static_cast<Uint32>(upload_size_64);

        SDL_GPUTextureCreateInfo info{};
        info.type = SDL_GPU_TEXTURETYPE_2D;
        info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        info.width = width;
        info.height = height;
        info.layer_count_or_depth = 1;
        info.num_levels = generate_mipmaps ? math::mipLevels(width, height) : 1;
        info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* texture = SDL_CreateGPUTexture(m_device, &info);
        if (!texture)
            throw std::runtime_error(std::string("SDL_CreateGPUTexture failed: ") + SDL_GetError());

        SDL_GPUTransferBufferCreateInfo transfer_info{};
        transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_info.size = upload_size;

        SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(m_device, &transfer_info);
        if (!transfer_buffer) {
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_CreateGPUTransferBuffer texture failed: ") + SDL_GetError());
        }

        void* mapped = SDL_MapGPUTransferBuffer(m_device, transfer_buffer, false);
        if (!mapped) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_MapGPUTransferBuffer texture failed: ") + SDL_GetError());
        }

        std::memcpy(mapped, pixels, upload_size);
        SDL_UnmapGPUTransferBuffer(m_device, transfer_buffer);

        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!command_buffer) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_AcquireGPUCommandBuffer in createTexture failed: ") + SDL_GetError());
        }

        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);
        if (!copy_pass) {
            SDL_CancelGPUCommandBuffer(command_buffer);
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_BeginGPUCopyPass in createTexture failed: ") + SDL_GetError());
        }

        SDL_GPUTextureTransferInfo source{};
        source.transfer_buffer = transfer_buffer;
        source.offset = 0;
        source.pixels_per_row = width;
        source.rows_per_layer = height;

        SDL_GPUTextureRegion destination{};
        destination.texture = texture;
        destination.w = width;
        destination.h = height;
        destination.d = 1;

        SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);
        SDL_EndGPUCopyPass(copy_pass);

        if (generate_mipmaps && info.num_levels > 1)
            SDL_GenerateMipmapsForGPUTexture(command_buffer, texture);

        if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createTexture failed: ") + SDL_GetError());
        }

        SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);

        return Texture{ m_device, texture, width, height };
    }

    RenderMesh RendererResources::createRenderMesh(const Mesh& mesh) {
        const auto& vertices = mesh.vertices();
        const auto& indices = mesh.indices();

        if (vertices.empty() || indices.empty())
            throw std::invalid_argument("Cannot create a render mesh from empty geometry");

        const std::size_t vertexByteSize = vertices.size() * sizeof(Vertex);
        const std::size_t indexByteSize = indices.size() * sizeof(std::uint32_t);
        const std::size_t totalByteSize = vertexByteSize + indexByteSize;

        if (totalByteSize > std::numeric_limits<Uint32>::max())
            throw std::length_error("Mesh is too large for an SDL GPU buffer");

        const Uint32 vertexBufferSize = static_cast<Uint32>(vertexByteSize);
        const Uint32 indexBufferSize = static_cast<Uint32>(indexByteSize);

        SDL_GPUBufferCreateInfo vertexBufferInfo{};
        vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vertexBufferInfo.size = vertexBufferSize;

        SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(m_device, &vertexBufferInfo);

        if (!vertexBuffer)
            throw std::runtime_error(std::string("Failed to create vertex buffer: ") + SDL_GetError());

        SDL_GPUBufferCreateInfo indexBufferInfo{};
        indexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        indexBufferInfo.size = indexBufferSize;

        SDL_GPUBuffer* indexBuffer = SDL_CreateGPUBuffer(m_device, &indexBufferInfo);

        if (!indexBuffer) {
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("Failed to create index buffer: ") + SDL_GetError());
        }

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferInfo.size = vertexBufferSize + indexBufferSize;

        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);

        if (!transferBuffer) {
            SDL_ReleaseGPUBuffer(m_device, indexBuffer);
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("Failed to create transfer buffer: ") + SDL_GetError());
        }

        void* mapped = SDL_MapGPUTransferBuffer(m_device, transferBuffer, false);

        if (!mapped) {
            SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
            SDL_ReleaseGPUBuffer(m_device, indexBuffer);
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("Failed to map transfer buffer: ") + SDL_GetError());
        }

        std::memcpy(mapped, vertices.data(), vertexBufferSize);
        std::memcpy(static_cast<char*>(mapped) + vertexBufferSize, indices.data(), indexBufferSize);

        SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

        if (!commandBuffer) {
            SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
            SDL_ReleaseGPUBuffer(m_device, indexBuffer);
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("Failed to acquire upload command buffer: ") + SDL_GetError());
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

        if (!copyPass) {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
            SDL_ReleaseGPUBuffer(m_device, indexBuffer);
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("Failed to begin GPU copy pass: ") + SDL_GetError());
        }

        SDL_GPUTransferBufferLocation vertexSource{};
        vertexSource.transfer_buffer = transferBuffer;
        vertexSource.offset = 0;

        SDL_GPUBufferRegion vertexDestination{};
        vertexDestination.buffer = vertexBuffer;
        vertexDestination.offset = 0;
        vertexDestination.size = vertexBufferSize;

        SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);

        SDL_GPUTransferBufferLocation indexSource{};
        indexSource.transfer_buffer = transferBuffer;
        indexSource.offset = vertexBufferSize;

        SDL_GPUBufferRegion indexDestination{};
        indexDestination.buffer = indexBuffer;
        indexDestination.offset = 0;
        indexDestination.size = indexBufferSize;

        SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);

        SDL_EndGPUCopyPass(copyPass);
        if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
            SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
            SDL_ReleaseGPUBuffer(m_device, indexBuffer);
            SDL_ReleaseGPUBuffer(m_device, vertexBuffer);
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createRenderMesh failed: ") + SDL_GetError());
        }

        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);

        AABB bounds;
        for (const auto& vertex : vertices)
            bounds.expand(vertex.position);

        return RenderMesh(m_device, vertexBuffer, indexBuffer, static_cast<std::uint32_t>(indices.size()), bounds);
    }

    Cubemap RendererResources::createCubemap(std::array<std::vector<std::uint8_t>, 6>& faces, std::uint32_t size) {
        if (size == 0)
            throw std::invalid_argument("Cannot create a cubemap with zero dimensions");

        constexpr Uint32 bytes_per_pixel = 4;
        const std::uint64_t face_size_64 = static_cast<std::uint64_t>(size) * size * bytes_per_pixel;
        const std::uint64_t upload_size_64 = face_size_64 * faces.size();
        if (face_size_64 > std::numeric_limits<Uint32>::max() || upload_size_64 > std::numeric_limits<Uint32>::max())
            throw std::length_error("Cubemap is too large for an SDL GPU transfer buffer");

        const Uint32 face_size = static_cast<Uint32>(face_size_64);
        const Uint32 upload_size = static_cast<Uint32>(upload_size_64);

        for (const auto& face : faces) {
            if (face.size() < face_size)
                throw std::invalid_argument("Cubemap face data is smaller than the requested cubemap size");
        }

        SDL_GPUTextureCreateInfo info{};
        info.type = SDL_GPU_TEXTURETYPE_CUBE;
        info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        info.width = size;
        info.height = size;
        info.layer_count_or_depth = 6;
        info.num_levels = math::mipLevels(size, size);
        info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* texture = SDL_CreateGPUTexture(m_device, &info);
        if (!texture)
            throw std::runtime_error(std::string("SDL_CreateGPUTexture cubemap failed: ") + SDL_GetError());

        SDL_GPUTransferBufferCreateInfo transfer_info{};
        transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_info.size = upload_size;

        SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(m_device, &transfer_info);
        if (!transfer_buffer) {
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_CreateGPUTransferBuffer cubemap failed: ") + SDL_GetError());
        }

        void* mapped = SDL_MapGPUTransferBuffer(m_device, transfer_buffer, false);
        if (!mapped) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_MapGPUTransferBuffer cubemap failed: ") + SDL_GetError());
        }

        for (std::size_t face = 0; face < 6; face++)
            std::memcpy(static_cast<std::uint8_t*>(mapped) + face * face_size, faces[face].data(), face_size);

        SDL_UnmapGPUTransferBuffer(m_device, transfer_buffer);

        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!command_buffer) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_AcquireGPUCommandBuffer in createCubemap failed: ") + SDL_GetError());
        }

        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);
        if (!copy_pass) {
            SDL_CancelGPUCommandBuffer(command_buffer);
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_BeginGPUCopyPass in createCubemap failed: ") + SDL_GetError());
        }

        for(std::uint32_t face = 0; face < 6; face++) {
            SDL_GPUTextureTransferInfo source{};
            source.transfer_buffer = transfer_buffer;
            source.offset = face * face_size;
            source.pixels_per_row = size;
            source.rows_per_layer = size;

            SDL_GPUTextureRegion destination{};
            destination.texture = texture;
            destination.layer = face;
            destination.w = size;
            destination.h = size;
            destination.d = 1;

            SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);
        }

        SDL_EndGPUCopyPass(copy_pass);

        SDL_GenerateMipmapsForGPUTexture(command_buffer, texture);

        if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_device, texture);
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createCubemap failed: ") + SDL_GetError());
        }

        SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
        return Cubemap{ m_device, texture, size };
    }
}
