#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "uniforms/VertexUniforms.hpp"
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace engine {
    Renderer::Renderer(Window& window): m_window(window) {
        m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, true, nullptr);
        if (!m_device)
            throw std::runtime_error(std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());

        if(!SDL_ClaimWindowForGPUDevice(m_device, m_window.handle()))
            throw std::runtime_error(std::string("SDL_ClaimWindowForGPUDevice failed: ") + SDL_GetError());

        Shader vertex_shader(m_device, "assets/shaders/triangle.vert.msl", ShaderStage::Vertex);
        Shader fragment_shader(m_device, "assets/shaders/triangle.frag.msl", ShaderStage::Fragment);

        SDL_GPUVertexInputState vertex_input_state = VertexLayout<Vertex>::create();

        SDL_GPUColorTargetDescription color_target_desc{};
        color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window.handle());

        SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.vertex_shader = vertex_shader.handle();
        pipeline_info.fragment_shader = fragment_shader.handle();
        pipeline_info.vertex_input_state = vertex_input_state;
        pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipeline_info.target_info.num_color_targets = 1;
        pipeline_info.target_info.color_target_descriptions = &color_target_desc;
        pipeline_info.target_info.has_depth_stencil_target = true;
        pipeline_info.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;

        pipeline_info.depth_stencil_state.enable_depth_test = true;
        pipeline_info.depth_stencil_state.enable_depth_write = true;
        pipeline_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;

        m_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipeline_info);

        if (!m_pipeline)
            throw std::runtime_error(std::string("SDL_CreateGPUGraphicsPipeline failed: ") + SDL_GetError());
    }

    Renderer::~Renderer() {
        if (!m_device)
            return;

        if (m_pipeline) {
            SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
            m_pipeline = nullptr;
        }

        if (m_depth_texture) {
            SDL_ReleaseGPUTexture(m_device, m_depth_texture);
            m_depth_texture = nullptr;
        }

        SDL_ReleaseWindowFromGPUDevice(m_device, m_window.handle());
        SDL_DestroyGPUDevice(m_device);
    }

    SDL_GPUDevice* Renderer::device() const {
        return m_device;
    }

    void Renderer::createDepthTexture(Uint32 width, Uint32 height) {
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

    RenderMesh Renderer::createRenderMesh(const Mesh& mesh) {
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
            throw std::runtime_error(std::string("Failed to submit mesh upload: ") + SDL_GetError());
        }

        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);

        return RenderMesh(m_device, vertexBuffer, indexBuffer, static_cast<std::uint32_t>(indices.size()));
    }

    void Renderer::draw(const RenderMesh& mesh, const Transform& transform, const Camera& camera) {
        if (!m_render_pass)
            throw std::logic_error("Renderer::draw called outside an active render pass");

        float aspect_ration = static_cast<float>(m_window.width()) / static_cast<float>(m_window.height());

        VertexUniforms uniforms{};
        uniforms.model = transform.matrix();
        uniforms.view = camera.viewMatrix();
        uniforms.projection = camera.projectionMatrix(aspect_ration);

        SDL_PushGPUVertexUniformData(m_command_buffer, 0, &uniforms, sizeof(VertexUniforms));

        SDL_BindGPUGraphicsPipeline(m_render_pass, m_pipeline);

        // Vertex buffer
        SDL_GPUBufferBinding vertex_binding{};
        vertex_binding.buffer = mesh.vertexBuffer();
        vertex_binding.offset = 0;

        SDL_BindGPUVertexBuffers(m_render_pass, 0, &vertex_binding, 1);

        // Index buffer
        SDL_GPUBufferBinding index_binding{};
        index_binding.buffer = mesh.indexBuffer();
        index_binding.offset = 0;

        SDL_BindGPUIndexBuffer(m_render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(m_render_pass, mesh.indexCount(), 1, 0, 0, 0);
    }

    void Renderer::beginFrame() {
        if (m_command_buffer)
            throw std::logic_error("Renderer::beginFrame called while a frame is active");

        m_command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!m_command_buffer)
            throw std::runtime_error(std::string("SDL_AcquireGPUCommandBuffer failed: ") + SDL_GetError());

        m_swapchain_texture = nullptr;
        Uint32 width = 0;
        Uint32 height = 0;

        if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_command_buffer, m_window.handle(), &m_swapchain_texture, &width, &height))
            throw std::runtime_error(std::string("SDL_WaitAndAcquireGPUSwapchainTexture failed: ") + SDL_GetError());

        if (!m_swapchain_texture) {
            SDL_SubmitGPUCommandBuffer(m_command_buffer);
            m_command_buffer = nullptr;
            return;
        }

        if (!m_depth_texture || width != m_depth_width || height != m_depth_height)
            createDepthTexture(width, height);
    }

    void Renderer::beginRenderPass() {
        if (!m_command_buffer || !m_swapchain_texture)
            return;

        if (m_render_pass)
            throw std::logic_error("Renderer::beginRenderPass called while a render pass is active");

        SDL_GPUColorTargetInfo color_target{};
        color_target.texture = m_swapchain_texture;
        color_target.clear_color = SDL_FColor{0.08f, 0.10f, 0.14f, 1.0f};
        color_target.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depth_target{};
        depth_target.texture = m_depth_texture;
        depth_target.clear_depth = 1.0f;
        depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
        depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
        depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        m_render_pass = SDL_BeginGPURenderPass(m_command_buffer, &color_target, 1, &depth_target);

        if (!m_render_pass) throw std::runtime_error(std::string("SDL_BeginGPURenderPass failed: ") + SDL_GetError());
    }

    void Renderer::endRenderPass() {
        if (!m_render_pass)
            return;

        SDL_EndGPURenderPass(m_render_pass);
        m_render_pass = nullptr;
    }

    void Renderer::endFrame() {
        if (m_render_pass)
            throw std::logic_error("Renderer::endFrame called with an active render pass");

        if (m_command_buffer) {
            if (!SDL_SubmitGPUCommandBuffer(m_command_buffer)) {
                m_command_buffer = nullptr;
                m_swapchain_texture = nullptr;
                throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer failed: ") + SDL_GetError());
            }
            m_command_buffer = nullptr;
        }
        m_swapchain_texture = nullptr;
    }
}
