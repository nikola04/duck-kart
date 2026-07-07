#pragma once

#include <SDL3/SDL_gpu.h>

#include <filesystem>
namespace engine {
    struct GraphicsPipelineInfo {
        SDL_GPUTextureFormat colorFormat;
        SDL_GPUTextureFormat depthFormat;

        int fragmentSamplers = 0;
        int vertexUniformBuffers = 1;
        int fragmentUniformBuffers = 4;

        bool blend = false;
        bool depthTest = true;
        bool depthWrite = true;
        SDL_GPUCompareOp compareOp = SDL_GPU_COMPAREOP_LESS;
    };

    class GraphicsPipeline {
        public:
            GraphicsPipeline(SDL_GPUDevice* device, const std::filesystem::path& vertex, const std::filesystem::path& fragment, GraphicsPipelineInfo info);
            ~GraphicsPipeline();

            SDL_GPUGraphicsPipeline* handle() const;

            GraphicsPipeline(const GraphicsPipeline&) = delete;
            GraphicsPipeline& operator =(const GraphicsPipeline&) = delete;
            GraphicsPipeline(GraphicsPipeline&&) noexcept = default;
            GraphicsPipeline& operator=(GraphicsPipeline&&) noexcept = default;

            void bind(SDL_GPURenderPass* pass) const;

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUGraphicsPipeline* m_pipeline = nullptr;
    };
}
