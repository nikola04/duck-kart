#include "Renderer.hpp"
#include "ShadowConstants.hpp"
#include "ShadowMap.hpp"
#include "uniforms/CameraUniforms.hpp"
#include "uniforms/MaterialUniforms.hpp"
#include "uniforms/ShadowLightUniforms.hpp"
#include "uniforms/ShadowVertexUniforms.hpp"
#include "uniforms/VertexUniforms.hpp"
#include "../math/Math.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <optional>

namespace engine {
    Renderer::Renderer(Window& window): m_window(window) {
        m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, true, nullptr);
        if (!m_device)
            throw std::runtime_error(std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());

        if(!SDL_ClaimWindowForGPUDevice(m_device, m_window.handle()))
            throw std::runtime_error(std::string("SDL_ClaimWindowForGPUDevice failed: ") + SDL_GetError());

        m_mainPipeline.emplace(m_device, "assets/shaders/default.vert.msl", "assets/shaders/default.frag.msl", GraphicsPipelineInfo{
            .colorFormat = SDL_GetGPUSwapchainTextureFormat(m_device, m_window.handle()),
            .depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .fragmentSamplers = 2 + ShadowCascadeCount,
            .vertexUniformBuffers = 2,
            .fragmentUniformBuffers = 4
        });

        m_skyboxPipeline.emplace(m_device, "assets/shaders/skybox.vert.msl", "assets/shaders/skybox.frag.msl", GraphicsPipelineInfo{
            .colorFormat = SDL_GetGPUSwapchainTextureFormat(m_device, m_window.handle()),
            .depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .fragmentSamplers = 1,
            .vertexUniformBuffers = 1,
            .fragmentUniformBuffers = 0,
            .depthTest = true,
            .depthWrite = false,
            .compareOp = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
        });

        m_shadowPipeline.emplace(m_device, "assets/shaders/shadow.vert.msl", "assets/shaders/shadow.frag.msl", GraphicsPipelineInfo{
            .colorFormat = SDL_GPU_TEXTUREFORMAT_INVALID,
            .depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .fragmentSamplers = 0,
            .vertexUniformBuffers = 1,
            .fragmentUniformBuffers = 0,
            .depthTest = true,
            .depthWrite = true,
            .compareOp = SDL_GPU_COMPAREOP_LESS
        });

        const Uint32 shadowSizes[ShadowCascadeCount] = { 4096, 2048 };
        for (std::size_t i = 0; i < ShadowCascadeCount; ++i)
            m_shadowMaps[i].emplace(m_device, shadowSizes[i], shadowSizes[i]);

        SDL_GPUSamplerCreateInfo sampler_info{};
        sampler_info.min_filter = SDL_GPU_FILTER_LINEAR;
        sampler_info.mag_filter = SDL_GPU_FILTER_LINEAR;
        sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        sampler_info.max_lod = 1000.0f;

        sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

        sampler_info.enable_anisotropy = true;
        sampler_info.max_anisotropy = 16.0f;

        m_default_sampler = SDL_CreateGPUSampler(m_device, &sampler_info);
        if (!m_default_sampler)
            throw std::runtime_error(std::string("SDL_CreateGPUSampler failed: ") + SDL_GetError());

        SDL_GPUSamplerCreateInfo shadow_sampler_info{};
        shadow_sampler_info.min_filter = SDL_GPU_FILTER_LINEAR;
        shadow_sampler_info.mag_filter = SDL_GPU_FILTER_LINEAR;
        shadow_sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;

        shadow_sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        shadow_sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        shadow_sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

        m_shadow_sampler = SDL_CreateGPUSampler(m_device, &shadow_sampler_info);
        if (!m_shadow_sampler)
            throw std::runtime_error(std::string("SDL_CreateGPUSampler shadow failed: ") + SDL_GetError());

        const std::uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        m_white_texture = std::make_unique<Texture>(createTexture(whitePixel, 1, 1, false));

        const std::uint8_t normalPixels[4] = { 128, 128, 255, 255 };
        m_default_normal_texture = std::make_unique<Texture>(createTexture(normalPixels, 1, 1, false));

        m_skyboxMesh.emplace(createRenderMesh({
            std::vector<Vertex>{
                {{-1, -1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1, -1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1,  1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{-1,  1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},

                {{-1, -1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1, -1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1,  1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{-1,  1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
            },
            std::vector<std::uint32_t>{
                0, 1, 2, 2, 3, 0,
                5, 4, 7, 7, 6, 5,
                4, 0, 3, 3, 7, 4,
                1, 5, 6, 6, 2, 1,
                3, 2, 6, 6, 7, 3,
                4, 5, 1, 1, 0, 4
            }
        }));
    }

    Renderer::~Renderer() {
        if (!m_device)
            return;

        SDL_WaitForGPUIdle(m_device);
        m_skyboxMesh.reset();
        m_white_texture.reset();
        m_default_normal_texture.reset();

        for (auto& shadowMap : m_shadowMaps)
            shadowMap.reset();

        m_mainPipeline.reset();
        m_skyboxPipeline.reset();
        m_shadowPipeline.reset();

        if (m_depth_texture) {
            SDL_ReleaseGPUTexture(m_device, m_depth_texture);
            m_depth_texture = nullptr;
        }

        if (m_default_sampler){
            SDL_ReleaseGPUSampler(m_device, m_default_sampler);
            m_default_sampler = nullptr;
        }

        if (m_shadow_sampler){
            SDL_ReleaseGPUSampler(m_device, m_shadow_sampler);
            m_shadow_sampler = nullptr;
        }

        SDL_ReleaseWindowFromGPUDevice(m_device, m_window.handle());
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
    }

    SDL_GPUDevice* Renderer::device() const {
        return m_device;
    }

    SDL_GPUSampler* Renderer::defaultSampler() const {
        return m_default_sampler;
    }

    Cubemap Renderer::createCubemap(std::array<std::vector<std::uint8_t>, 6>& faces, std::uint32_t size) {
        const Uint32 bytes_per_pixel = 4;
        const Uint32 face_size = size * size * bytes_per_pixel;
        const Uint32 upload_size = face_size * 6;

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

        void* mapped = SDL_MapGPUTransferBuffer(m_device, transfer_buffer, false);

        for (std::size_t face = 0; face < 6; face++)
            std::memcpy(static_cast<std::uint8_t*>(mapped) + face * face_size, faces[face].data(), face_size);

        SDL_UnmapGPUTransferBuffer(m_device, transfer_buffer);

        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);

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

        if (!SDL_SubmitGPUCommandBuffer(command_buffer))
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createCubemap failed: ") + SDL_GetError());

        SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);
        return Cubemap{ m_device, texture, size };
    }

    Texture Renderer::createTexture(const void* pixels, Uint32 width, Uint32 height, bool generate_mipmaps) {
        const Uint32 bytes_per_pixel = 4, upload_size = width * height * bytes_per_pixel;

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
        if (!transfer_buffer)
            throw std::runtime_error(std::string("SDL_CreateGPUTransferBuffer texture failed: ") + SDL_GetError());

        void* mapped = SDL_MapGPUTransferBuffer(m_device, transfer_buffer, false);
        std::memcpy(mapped, pixels, upload_size);
        SDL_UnmapGPUTransferBuffer(m_device, transfer_buffer);

        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);

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

        if (!SDL_SubmitGPUCommandBuffer(command_buffer))
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createTexture failed: ") + SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);

        return Texture{ m_device, texture, width, height };
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
            throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in createRenderMesh failed: ") + SDL_GetError());
        }

        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);

        return RenderMesh(m_device, vertexBuffer, indexBuffer, static_cast<std::uint32_t>(indices.size()));
    }

    void Renderer::render(const Scene& scene) {
        const float splits[ShadowCascadeCount] = { 50.0f, 250.0f };
        const float extents[ShadowCascadeCount] = { 50.0f, 250.0f };

        glm::vec3 camPos = scene.camera.transform.position;
        glm::vec3 forward = scene.camera.transform.forward();

        for (std::size_t i = 0; i < ShadowCascadeCount; i++) {
            float nearSplit = (i == 0) ? 0.0f : splits[i - 1];
            float farSplit = splits[i];

            float centerDistance = (nearSplit + farSplit) * 0.5f;
            glm::vec3 target = camPos + forward * centerDistance;

            m_shadowCameras[i].update(
                scene.sun,
                target,
                extents[i],
                static_cast<float>(m_shadowMaps[i]->width())
            );

            beginShadowPass(i);

            for (const auto& object : scene.objects)
                drawShadow(*object.mesh, object.transform, i);

            endShadowPass();
        }

        beginRenderPass();
        PointLightUniforms pointLightUniforms{};
        std::size_t count = std::min(scene.pointLights.size(), static_cast<std::size_t>(MaxPointLights));
        pointLightUniforms.count.x = static_cast<float>(count);

        for (std::size_t i = 0; i < count; ++i)
            pointLightUniforms.lights[i] = scene.pointLights[i];

        for (const auto& object : scene.objects)
            draw(*object.mesh, object.transform, scene.camera, *object.material, scene.sun, pointLightUniforms);

        drawSkybox(scene.skybox, scene.camera);
        endRenderPass();
    }

    void Renderer::drawSkybox(const Skybox& skybox, const Camera& camera) {
        if (!m_render_pass)
            throw std::logic_error("Renderer::drawSkybox called outside an active render pass");

        if (!skybox.cubemap)
            return;

        m_skyboxPipeline->bind(m_render_pass);

        SDL_GPUBufferBinding vertex_binding{};
        vertex_binding.buffer = m_skyboxMesh->vertexBuffer();
        vertex_binding.offset = 0;

        SDL_BindGPUVertexBuffers(m_render_pass, 0, &vertex_binding, 1);

        SDL_GPUBufferBinding index_binding{};
        index_binding.buffer = m_skyboxMesh->indexBuffer();
        index_binding.offset = 0;

        SDL_BindGPUIndexBuffer(m_render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        float aspectRatio =
            static_cast<float>(m_window.width()) /
            static_cast<float>(m_window.height());

        struct SkyboxUniforms {
            glm::mat4 view;
            glm::mat4 projection;
        };

        SkyboxUniforms uniforms{};
        uniforms.view = camera.viewMatrix();
        uniforms.projection = camera.projectionMatrix(aspectRatio);

        SDL_PushGPUVertexUniformData(m_command_buffer, 0, &uniforms, sizeof(uniforms));

        SDL_GPUTextureSamplerBinding binding{};
        binding.texture = skybox.cubemap->handle();
        binding.sampler = m_default_sampler;

        SDL_BindGPUFragmentSamplers(m_render_pass, 0, &binding, 1);

        SDL_DrawGPUIndexedPrimitives(m_render_pass, m_skyboxMesh->indexCount(), 1, 0, 0, 0);
    }

    void Renderer::draw(const RenderMesh& mesh, const Transform& transform, const Camera& camera, const Material& material, const DirectionalLight& light, const PointLightUniforms& pointLights) {
        if (!m_render_pass)
            throw std::logic_error("Renderer::draw called outside an active render pass");

        float aspect_ration = static_cast<float>(m_window.width()) / static_cast<float>(m_window.height());

        VertexUniforms uniforms{};
        uniforms.model = transform.matrix();
        uniforms.view = camera.viewMatrix();
        uniforms.projection = camera.projectionMatrix(aspect_ration);
        SDL_PushGPUVertexUniformData(m_command_buffer, 0, &uniforms, sizeof(VertexUniforms));

        ShadowLightUniforms shadow_uniforms{};
        for (std::size_t i = 0; i < ShadowCascadeCount; ++i) {
            shadow_uniforms.lightVP[i] = m_shadowCameras[i].projection() * m_shadowCameras[i].view();
        }
        shadow_uniforms.cascadeSplits = glm::vec4(50.0f, 0.0f, 0.0f, 0.0f);
        SDL_PushGPUVertexUniformData(m_command_buffer, 1, &shadow_uniforms, sizeof(ShadowLightUniforms));

        MaterialUniforms material_uniforms{};
        material_uniforms.base_color = material.baseColor;
        material_uniforms.properties = { material.metallic, material.roughness, material.alphaCutoff, material.alphaMode };
        SDL_PushGPUFragmentUniformData(m_command_buffer, 0, &material_uniforms, sizeof(MaterialUniforms));

        CameraUniforms camera_uniforms{};
        camera_uniforms.position = glm::vec4{camera.transform.position, 50.0f};
        SDL_PushGPUFragmentUniformData(m_command_buffer, 1, &camera_uniforms, sizeof(CameraUniforms));
        SDL_PushGPUFragmentUniformData(m_command_buffer, 2, &light, sizeof(DirectionalLight));
        SDL_PushGPUFragmentUniformData(m_command_buffer, 3, &pointLights, sizeof(PointLightUniforms));


        m_mainPipeline->bind(m_render_pass);

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

        const Texture* texture_to_bind = material.texture ? material.texture : m_white_texture.get();
        const Texture* normal_texture_to_bind = material.normalTexture ? material.normalTexture : m_default_normal_texture.get();

        constexpr std::size_t bindings_count = 2 + ShadowCascadeCount;
        SDL_GPUTextureSamplerBinding bindings[bindings_count] = {
            { .texture = texture_to_bind->handle(), .sampler = m_default_sampler },
            { .texture = normal_texture_to_bind->handle(), .sampler = m_default_sampler },
        };

        std::size_t offset = bindings_count - ShadowCascadeCount;
        for (std::size_t i = 0; i < ShadowCascadeCount; i++)
            bindings[offset + i] = { .texture = m_shadowMaps[i]->texture(), .sampler = m_shadow_sampler };

        SDL_BindGPUFragmentSamplers(m_render_pass, 0, bindings, 2 + ShadowCascadeCount);

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
                throw std::runtime_error(std::string("SDL_SubmitGPUCommandBuffer in endFrame failed: ") + SDL_GetError());
            }
            m_command_buffer = nullptr;
        }
        m_swapchain_texture = nullptr;
    }

    void Renderer::beginShadowPass(std::size_t cascade) {
        if (!m_command_buffer)
            return;

        if (m_shadow_render_pass)
            throw std::logic_error("Renderer::beginShadowPass called while a render pass is active");

        SDL_GPUDepthStencilTargetInfo depth_target{};
        depth_target.texture = m_shadowMaps[cascade]->texture();
        depth_target.clear_depth = 1.0f;
        depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
        depth_target.store_op = SDL_GPU_STOREOP_STORE;
        depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        m_shadow_render_pass = SDL_BeginGPURenderPass(m_command_buffer, nullptr, 0, &depth_target);

        if (!m_shadow_render_pass) throw std::runtime_error(std::string("SDL_BeginGPURenderPass in beginShadowPass failed: ") + SDL_GetError());
    }

    void Renderer::endShadowPass() {
        if (!m_shadow_render_pass)
            return;

        SDL_EndGPURenderPass(m_shadow_render_pass);
        m_shadow_render_pass = nullptr;
    }

    void Renderer::drawShadow(const RenderMesh& mesh, const Transform& transform, std::size_t cascade) {
        if (!m_shadow_render_pass)
            throw std::logic_error("Renderer::drawShadow called outside an active shadow pass");

        ShadowVertexUniforms uniforms{};

        uniforms.model = transform.matrix();
        uniforms.lightView = m_shadowCameras[cascade].view();
        uniforms.lightProjection = m_shadowCameras[cascade].projection();

        SDL_PushGPUVertexUniformData(m_command_buffer, 0, &uniforms, sizeof(ShadowVertexUniforms));

        m_shadowPipeline->bind(m_shadow_render_pass);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = mesh.vertexBuffer();
        vertexBinding.offset = 0;

        SDL_BindGPUVertexBuffers(m_shadow_render_pass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = mesh.indexBuffer();
        indexBinding.offset = 0;

        SDL_BindGPUIndexBuffer(m_shadow_render_pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(m_shadow_render_pass, mesh.indexCount(), 1, 0, 0, 0);
    }
}
