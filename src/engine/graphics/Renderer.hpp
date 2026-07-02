#pragma once

#include <SDL3/SDL.h>
#include "../window/Window.hpp"
#include "../math/Transform.hpp"
#include "../scene/Camera.hpp"
#include "Mesh.hpp"
#include "RenderMesh.hpp"

namespace engine {
    class Renderer {
        public:
            Renderer(Window& window);
            ~Renderer();

            Renderer(const Renderer& renderer) = delete;
            Renderer& operator = (const Renderer& renderer) = delete;

            SDL_GPUDevice* device() const;

            RenderMesh createRenderMesh(const Mesh& mesh);

            void beginRenderPass();
            void draw(const RenderMesh& mesh, const Transform& transform, const Camera& camera);
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

            Uint32 m_depth_width = 0;
            Uint32 m_depth_height = 0;

            void createDepthTexture(Uint32 width, Uint32 height);
    };
}
