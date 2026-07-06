#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <glm/fwd.hpp>
#include <memory>
#include <optional>
#include "../window/Window.hpp"
#include "../math/Transform.hpp"
#include "../scene/Camera.hpp"
#include "../scene/Scene.hpp"
#include "Cubemap.hpp"
#include "GraphicsPipeline.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "RenderMesh.hpp"
#include "ShadowCamera.hpp"
#include "ShadowMap.hpp"
#include "Texture.hpp"
#include "uniforms/PointLightUniforms.hpp"

namespace engine {
    class Renderer {
        public:
            Renderer(Window& window);
            ~Renderer();

            Renderer(const Renderer& renderer) = delete;
            Renderer& operator = (const Renderer& renderer) = delete;

            SDL_GPUDevice* device() const;
            SDL_GPUSampler* defaultSampler() const;

            RenderMesh createRenderMesh(const Mesh& mesh);
            Texture createTexture(const void* pixels, Uint32 width, Uint32 height, bool generate_mipmaps = true);
            Cubemap createCubemap(std::array<std::vector<std::uint8_t>, 6>& faces, std::uint32_t size = 0);

            void drawShadow(const RenderMesh& mesh, const Transform& transform);
            void draw(
                const RenderMesh& mesh,
                const Transform& transform,
                const Camera& camera,
                const Material& material,
                const DirectionalLight& light,
                const PointLightUniforms& pointLights
            );
            void drawSkybox(const Skybox& skybox, const Camera& camera);
            void render(const Scene& scene);

            void beginFrame();
            void endFrame();

        private:
            Window& m_window;
            SDL_GPUDevice* m_device = nullptr;
            ShadowCamera m_shadowCamera;

            std::optional<GraphicsPipeline> m_mainPipeline;
            std::optional<GraphicsPipeline> m_skyboxPipeline;
            std::optional<GraphicsPipeline> m_shadowPipeline;

            std::optional<RenderMesh> m_skyboxMesh;
            std::optional<ShadowMap> m_shadowMap;

            SDL_GPUCommandBuffer* m_command_buffer = nullptr;
            SDL_GPUTexture* m_swapchain_texture = nullptr;
            SDL_GPUTexture* m_depth_texture = nullptr;
            SDL_GPURenderPass* m_render_pass = nullptr;
            SDL_GPURenderPass* m_shadow_render_pass = nullptr;
            SDL_GPUSampler* m_default_sampler = nullptr;

            std::unique_ptr<Texture> m_white_texture;
            std::unique_ptr<Texture> m_default_normal_texture;

            Uint32 m_depth_width = 0;
            Uint32 m_depth_height = 0;

            void createDepthTexture(Uint32 width, Uint32 height);

            void beginShadowPass();
            void endShadowPass();

            void beginRenderPass();
            void endRenderPass();
    };
}
