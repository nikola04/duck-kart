#include "Renderer.hpp"
#include "ShadowConstants.hpp"
#include "renderer/RendererGeometry.hpp"
#include <SDL3/SDL_gpu.h>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace engine {
    static SDL_GPUPresentMode choosePresentMode(SDL_GPUDevice* device, SDL_Window* window, bool vsync) {
        if (vsync) {
            if (SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_MAILBOX))
                return SDL_GPU_PRESENTMODE_MAILBOX;

            return SDL_GPU_PRESENTMODE_VSYNC;
        }

        if (SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_IMMEDIATE))
            return SDL_GPU_PRESENTMODE_IMMEDIATE;

        if (SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_MAILBOX))
            return SDL_GPU_PRESENTMODE_MAILBOX;

        return SDL_GPU_PRESENTMODE_VSYNC;
    }

    template<typename EndFn>
    class PassScope {
        public:
            explicit PassScope(EndFn end): m_end(end) {}
            ~PassScope() { m_end(); }

            PassScope(const PassScope&) = delete;
            PassScope& operator=(const PassScope&) = delete;

        private:
            EndFn m_end;
    };

    Renderer::Renderer(Window& window): m_window(window) {
        m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, true, nullptr);
        if (!m_device)
            throw std::runtime_error(std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());

        if (!SDL_ClaimWindowForGPUDevice(m_device, m_window.handle()))
            throw std::runtime_error(std::string("SDL_ClaimWindowForGPUDevice failed: ") + SDL_GetError());

        SDL_GPUPresentMode presentMode = choosePresentMode(m_device, m_window.handle(), m_graphicsSettings.vsyncEnabled);
        if (!SDL_SetGPUSwapchainParameters(m_device, m_window.handle(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode)) {
            throw std::runtime_error(std::string("SDL_SetGPUSwapchainParameters failed: ") + SDL_GetError());
        }

        m_resources.emplace(m_device);

        m_mainPipeline.emplace(m_device, "assets/shaders/default.vert.msl", "assets/shaders/default.frag.msl", GraphicsPipelineInfo{
            .colorFormat = SDL_GetGPUSwapchainTextureFormat(m_device, m_window.handle()),
            .depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .fragmentSamplers = 3 + ShadowCascadeCount,
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
            .fragmentSamplers = 1,
            .vertexUniformBuffers = 1,
            .fragmentUniformBuffers = 1,
            .depthTest = true,
            .depthWrite = true,
            .compareOp = SDL_GPU_COMPAREOP_LESS
        });

        m_uiPipeline.emplace(m_device, "assets/shaders/ui.vert.msl", "assets/shaders/ui.frag.msl", GraphicsPipelineInfo{
            .colorFormat = SDL_GetGPUSwapchainTextureFormat(m_device, m_window.handle()),
            .depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .fragmentSamplers = 1,
            .vertexUniformBuffers = 1,
            .fragmentUniformBuffers = 1,
            .blend = true,
            .depthTest = false,
            .depthWrite = false,
        });

        for (std::size_t i = 0; i < ShadowCascadeCount; ++i)
            m_shadowMaps[i].emplace(m_device, ShadowCascadeTextureSizes[i], ShadowCascadeTextureSizes[i]);

        SDL_GPUSamplerCreateInfo samplerInfo{};
        samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        samplerInfo.max_lod = 1000.0f;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.enable_anisotropy = m_graphicsSettings.anisotropyEnabled;
        samplerInfo.max_anisotropy = m_graphicsSettings.maxAnisotropy;

        m_default_sampler = SDL_CreateGPUSampler(m_device, &samplerInfo);
        if (!m_default_sampler)
            throw std::runtime_error(std::string("SDL_CreateGPUSampler failed: ") + SDL_GetError());

        SDL_GPUSamplerCreateInfo shadowSamplerInfo{};
        shadowSamplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
        shadowSamplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
        shadowSamplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        shadowSamplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        shadowSamplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        shadowSamplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

        m_shadow_sampler = SDL_CreateGPUSampler(m_device, &shadowSamplerInfo);
        if (!m_shadow_sampler)
            throw std::runtime_error(std::string("SDL_CreateGPUSampler shadow failed: ") + SDL_GetError());

        const std::uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        m_white_texture = std::make_unique<Texture>(resources().createTexture(whitePixel, 1, 1, false));

        const std::uint8_t normalPixels[4] = { 128, 128, 255, 255 };
        m_default_normal_texture = std::make_unique<Texture>(resources().createTexture(normalPixels, 1, 1, false));

        m_skyboxMesh.emplace(resources().createRenderMesh(createSkyboxMesh()));
        m_uiQuadMesh.emplace(resources().createRenderMesh(createQuad2D({0, 0}, {1, 1})));
    }

    Renderer::~Renderer() {
        if (!m_device)
            return;

        SDL_WaitForGPUIdle(m_device);

        m_skyboxMesh.reset();
        m_uiQuadMesh.reset();
        m_white_texture.reset();
        m_default_normal_texture.reset();

        for (auto& shadowMap : m_shadowMaps)
            shadowMap.reset();

        m_mainPipeline.reset();
        m_skyboxPipeline.reset();
        m_shadowPipeline.reset();
        m_uiPipeline.reset();
        m_resources.reset();

        if (m_default_sampler) {
            SDL_ReleaseGPUSampler(m_device, m_default_sampler);
            m_default_sampler = nullptr;
        }

        if (m_shadow_sampler) {
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

    RendererResources& Renderer::resources() {
        return *m_resources;
    }

    void Renderer::renderShadows(const Scene& scene) {
        float aspect = static_cast<float>(m_window.width()) / static_cast<float>(m_window.height());

        for (std::size_t i = 0; i < ShadowCascadeCount; i++) {
            float cascadeNear = (i == 0) ? 0.1f : ShadowCascadeSplits[i - 1];
            float cascadeFar = ShadowCascadeSplits[i];
            float shadowNear = std::max(0.1f, cascadeNear - ShadowCascadeOverlap);
            float shadowFar = cascadeFar + ShadowCascadeOverlap;

            m_shadowCameras[i].updateFromFrustumSlice(scene.sun, scene.camera, shadowNear, shadowFar, aspect, static_cast<float>(ShadowCascadeTextureSizes[i]));

            beginShadowPass(i);
            PassScope shadowPass{[this] { endShadowPass(); }};

            m_shadowRenderer.render(scene, ShadowRenderer::Context{
                .commandBuffer = m_command_buffer,
                .renderPass = m_shadow_render_pass,
                .shadowPipeline = *m_shadowPipeline,
                .defaultSampler = m_default_sampler,
                .whiteTexture = *m_white_texture,
                .shadowCameras = m_shadowCameras,
            }, i);
        }
    }

    void Renderer::renderScene(const Scene& scene) {
        beginRenderPass();
        PassScope renderPass{[this] { endRenderPass(); }};

        SceneRenderStats sceneStats = m_sceneRenderer.render(scene, SceneRenderer::Context{
            .window = m_window,
            .commandBuffer = m_command_buffer,
            .renderPass = m_render_pass,
            .mainPipeline = *m_mainPipeline,
            .skyboxPipeline = *m_skyboxPipeline,
            .skyboxMesh = *m_skyboxMesh,
            .defaultSampler = m_default_sampler,
            .shadowSampler = m_shadow_sampler,
            .whiteTexture = *m_white_texture,
            .defaultNormalTexture = *m_default_normal_texture,
            .shadowMaps = m_shadowMaps,
            .shadowCameras = m_shadowCameras,
        });

        m_stats.visibleChunks = sceneStats.visibleChunks;
        m_stats.visibleObjects = sceneStats.visibleObjects;
        m_stats.drawCalls = sceneStats.drawCalls;

        m_uiRenderer.render(scene.uiTextures, UIRenderer::Context{
            .window = m_window,
            .commandBuffer = m_command_buffer,
            .renderPass = m_render_pass,
            .uiPipeline = *m_uiPipeline,
            .quadMesh = *m_uiQuadMesh,
            .defaultSampler = m_default_sampler,
        });
    }

    void Renderer::render(const Scene& scene) {
        if (!m_command_buffer || !m_swapchain_texture)
              return;

        if (m_graphicsSettings.shadowsEnabled)
            renderShadows(scene);

        renderScene(scene);
    }

    const RendererStats& Renderer::stats() const {
        return m_stats;
    }

    bool Renderer::beginFrame() {
        if (m_command_buffer)
            throw std::logic_error("Renderer::beginFrame called while a frame is active");

        m_command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!m_command_buffer)
            throw std::runtime_error(std::string("SDL_AcquireGPUCommandBuffer failed: ") + SDL_GetError());

        m_swapchain_texture = nullptr;
        Uint32 width = 0;
        Uint32 height = 0;

        if (!SDL_AcquireGPUSwapchainTexture(m_command_buffer, m_window.handle(), &m_swapchain_texture, &width, &height))
            throw std::runtime_error(std::string("SDL_AcquireGPUSwapchainTexture failed: ") + SDL_GetError());

        if (!m_swapchain_texture) {
            SDL_SubmitGPUCommandBuffer(m_command_buffer);
            m_command_buffer = nullptr;
            return false;
        }

        m_resources->ensureDepthTexture(width, height);
        return true;
    }

    void Renderer::beginRenderPass() {
        if (!m_command_buffer || !m_swapchain_texture)
            throw std::logic_error("Renderer::beginRenderPass called outside an active frame");

        if (m_render_pass)
            throw std::logic_error("Renderer::beginRenderPass called while a render pass is active");

        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = m_swapchain_texture;
        colorTarget.clear_color = SDL_FColor{0.08f, 0.10f, 0.14f, 1.0f};
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthTarget{};
        depthTarget.texture = m_resources->depthTexture();
        depthTarget.clear_depth = 1.0f;
        depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
        depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        m_render_pass = SDL_BeginGPURenderPass(m_command_buffer, &colorTarget, 1, &depthTarget);
        if (!m_render_pass)
            throw std::runtime_error(std::string("SDL_BeginGPURenderPass failed: ") + SDL_GetError());
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
            throw std::logic_error("Renderer::beginShadowPass called outside an active frame");

        if (m_shadow_render_pass)
            throw std::logic_error("Renderer::beginShadowPass called while a render pass is active");

        SDL_GPUDepthStencilTargetInfo depthTarget{};
        depthTarget.texture = m_shadowMaps[cascade]->texture();
        depthTarget.clear_depth = 1.0f;
        depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        depthTarget.store_op = SDL_GPU_STOREOP_STORE;
        depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
        depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        m_shadow_render_pass = SDL_BeginGPURenderPass(m_command_buffer, nullptr, 0, &depthTarget);
        if (!m_shadow_render_pass)
            throw std::runtime_error(std::string("SDL_BeginGPURenderPass in beginShadowPass failed: ") + SDL_GetError());
    }

    void Renderer::endShadowPass() {
        if (!m_shadow_render_pass)
            return;

        SDL_EndGPURenderPass(m_shadow_render_pass);
        m_shadow_render_pass = nullptr;
    }
}
