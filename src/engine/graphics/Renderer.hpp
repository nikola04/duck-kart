#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <cstddef>
#include <glm/fwd.hpp>
#include <memory>
#include <optional>
#include "../window/Window.hpp"
#include "../scene/Scene.hpp"
#include "../settings/EngineSettings.hpp"
#include "GraphicsPipeline.hpp"
#include "ShadowCamera.hpp"
#include "ShadowConstants.hpp"
#include "ShadowMap.hpp"
#include "Texture.hpp"
#include "renderer/RendererResources.hpp"
#include "renderer/RenderMesh.hpp"
#include "renderer/SceneRenderer.hpp"
#include "renderer/ShadowRenderer.hpp"
#include "renderer/UIRenderer.hpp"
#include <array>

namespace engine {
    struct RendererStats {
        std::size_t visibleChunks = 0;
        std::size_t visibleObjects = 0;
        std::size_t drawCalls = 0;
    };

    class Renderer {
        public:
            Renderer(Window& window);
            ~Renderer();

            Renderer(const Renderer& renderer) = delete;
            Renderer& operator = (const Renderer& renderer) = delete;

            SDL_GPUDevice* device() const;
            SDL_GPUSampler* defaultSampler() const;
            RendererResources& resources();

            void render(const Scene& scene);
            const RendererStats& stats() const;

            bool beginFrame();
            void endFrame();

        private:
            Window& m_window;
            SDL_GPUDevice* m_device = nullptr;
            std::optional<RendererResources> m_resources;

            GraphicsSettings m_graphicsSettings = settings().graphics;

            std::optional<GraphicsPipeline> m_mainPipeline;
            std::optional<GraphicsPipeline> m_skyboxPipeline;
            std::optional<GraphicsPipeline> m_shadowPipeline;
            std::optional<GraphicsPipeline> m_uiPipeline;

            std::optional<RenderMesh> m_skyboxMesh;
            std::optional<RenderMesh> m_uiQuadMesh;

            std::array<std::optional<ShadowMap>, ShadowCascadeCount> m_shadowMaps;
            std::array<ShadowCamera, ShadowCascadeCount> m_shadowCameras;

            SDL_GPUCommandBuffer* m_command_buffer = nullptr;
            SDL_GPUTexture* m_swapchain_texture = nullptr;
            SDL_GPURenderPass* m_render_pass = nullptr;
            SDL_GPURenderPass* m_shadow_render_pass = nullptr;
            SDL_GPUSampler* m_default_sampler = nullptr;
            SDL_GPUSampler* m_shadow_sampler = nullptr;

            std::unique_ptr<Texture> m_white_texture;
            std::unique_ptr<Texture> m_default_normal_texture;

            SceneRenderer m_sceneRenderer;
            ShadowRenderer m_shadowRenderer;
            UIRenderer m_uiRenderer;
            RendererStats m_stats;

            // helpers
            void renderShadows(const Scene& scene);
            void renderScene(const Scene& scene);

            // lifecycle
            void beginRenderPass();
            void endRenderPass();
            void beginShadowPass(std::size_t cascade);
            void endShadowPass();
    };
}
