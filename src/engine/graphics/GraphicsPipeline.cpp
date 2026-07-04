#include "GraphicsPipeline.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include <SDL3/SDL_gpu.h>

namespace engine {
    GraphicsPipeline::GraphicsPipeline(SDL_GPUDevice* device, const std::filesystem::path& vertex, const std::filesystem::path& fragment, GraphicsPipelineInfo info): m_device(device) {
        Shader vertex_shader(m_device, vertex, ShaderStage::Vertex);
        Shader fragment_shader(m_device, fragment, ShaderStage::Fragment, info.fragmentSamplers);

        SDL_GPUVertexInputState vertex_input_state = VertexLayout<Vertex>::create();

        SDL_GPUColorTargetDescription color_target_desc{};
        color_target_desc.format = info.colorFormat;

        SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.vertex_shader = vertex_shader.handle();
        pipeline_info.fragment_shader = fragment_shader.handle();
        pipeline_info.vertex_input_state = vertex_input_state;
        pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipeline_info.target_info.num_color_targets = 1;
        pipeline_info.target_info.color_target_descriptions = &color_target_desc;
        pipeline_info.target_info.has_depth_stencil_target = true;

        pipeline_info.target_info.depth_stencil_format = info.depthFormat;
        pipeline_info.depth_stencil_state.enable_depth_test = info.depthTest;
        pipeline_info.depth_stencil_state.enable_depth_write = info.depthWrite;
        pipeline_info.depth_stencil_state.compare_op = info.compareOp;

        m_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipeline_info);

        if (!m_pipeline)
            throw std::runtime_error(std::string("SDL_CreateGPUGraphicsPipeline failed: ") + SDL_GetError());
    }

    GraphicsPipeline::~GraphicsPipeline() {
        if (m_pipeline)
            SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
    }

    void GraphicsPipeline::bind(SDL_GPURenderPass* pass) const {
        SDL_BindGPUGraphicsPipeline(pass, m_pipeline);
    }

    SDL_GPUGraphicsPipeline* GraphicsPipeline::handle() const {
        return m_pipeline;
    }
}
