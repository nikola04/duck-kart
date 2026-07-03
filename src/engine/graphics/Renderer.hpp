#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <glm/fwd.hpp>
#include "../window/Window.hpp"
#include "../math/Transform.hpp"
#include "../scene/Camera.hpp"
#include "Mesh.hpp"
#include "RenderMesh.hpp"
#include "Texture.hpp"

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
            Texture createTexture(const void* pixels, Uint32 width, Uint32 height);

            void beginRenderPass();
            void draw(
                const RenderMesh& mesh,
                const Transform& transform,
                const Camera& camera,
                const Texture* texture,
                const glm::vec4& base_color,
                float metallic,
                float roughness
            );
            void endRenderPass();

            void beginFrame();
            void endFrame();

        private:
            Window& m_window;

            SDL_GPUDevice* m_device = nullptr;
            SDL_GPUGraphicsPipeline* m_pipeline = nullptr;

            SDL_GPUCommandBuffer* m_command_buffer = nullptr;
            SDL_GPUTexture* m_swapchain_texture = nullptr;
            SDL_GPUTexture* m_depth_texture = nullptr;
            SDL_GPURenderPass* m_render_pass = nullptr;
            SDL_GPUSampler* m_default_sampler = nullptr;

            std::unique_ptr<Texture> m_white_texture;

            Uint32 m_depth_width = 0;
            Uint32 m_depth_height = 0;

            void createDepthTexture(Uint32 width, Uint32 height);
    };
}
