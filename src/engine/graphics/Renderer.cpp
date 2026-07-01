#include "Renderer.hpp"
#include <stdexcept>
#include <string>

namespace engine {
    Renderer::Renderer(Window& window): m_window(window) {
        SDL_GPUShaderFormat shader_flags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;

        m_device = SDL_CreateGPUDevice(shader_flags, true, nullptr);
        if (!m_device)
            throw std::runtime_error(std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());

        if(!SDL_ClaimWindowForGPUDevice(m_device, m_window.handle()))
            throw std::runtime_error(std::string("SDL_ClaimWindowForGPUDevice failed: ") + SDL_GetError());
    }

    Renderer::~Renderer() {
        if (!m_device) return;

        SDL_ReleaseWindowFromGPUDevice(m_device, m_window.handle());
        SDL_DestroyGPUDevice(m_device);
    }

    void Renderer::render() {
        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!command_buffer)
            throw std::runtime_error(std::string("SDL_AcquireGPUCommandBuffer failed: ") + SDL_GetError());

        SDL_GPUTexture* swapchain_texture = nullptr;
        Uint32 width = 0;
        Uint32 height = 0;

        if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, m_window.handle(), &swapchain_texture, &width, &height))
            throw std::runtime_error(std::string("SDL_WaitAndAcquireGPUSwapchainTexture failed: ") + SDL_GetError());

        if (!swapchain_texture) {
            SDL_SubmitGPUCommandBuffer(command_buffer);
            return;
        }

        SDL_GPUColorTargetInfo color_target{};
        color_target.texture = swapchain_texture;
        color_target.clear_color = SDL_FColor{0.08f, 0.10f, 0.14f, 1.0f};
        color_target.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &color_target, 1, nullptr);
        SDL_EndGPURenderPass(render_pass);
        SDL_SubmitGPUCommandBuffer(command_buffer);
    }
}
