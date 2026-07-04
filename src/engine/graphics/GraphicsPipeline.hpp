#pragma once

#include <SDL3/SDL_gpu.h>

#include <filesystem>
namespace engine {
    struct GraphicsPipelineInfo {
        SDL_GPUTextureFormat colorFormat;
        SDL_GPUTextureFormat depthFormat;
    };

    class GraphicsPipeline {
        public:
            GraphicsPipeline(SDL_GPUDevice* device, const std::filesystem::path& vertex, const std::filesystem::path& fragment, GraphicsPipelineInfo info);
            ~GraphicsPipeline();

            SDL_GPUGraphicsPipeline* handle() const;

            GraphicsPipeline(const GraphicsPipeline&) = delete;
            GraphicsPipeline& operator =(const GraphicsPipeline&) = delete;
            GraphicsPipeline(GraphicsPipeline&) noexcept = delete;
            GraphicsPipeline& operator =(GraphicsPipeline&) noexcept = delete;

            void bind(SDL_GPURenderPass* pass) const;

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUGraphicsPipeline* m_pipeline = nullptr;
    };
}
