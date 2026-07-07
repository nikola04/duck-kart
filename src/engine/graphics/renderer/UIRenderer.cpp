#include "UIRenderer.hpp"
#include <stdexcept>

namespace engine {
    void UIRenderer::render(const std::vector<UITexture>& textures, const Context& context) const {
        for (const auto& ui : textures) {
            if (!ui.texture)
                continue;

            drawTexture2D(*ui.texture, ui.position, ui.size, ui.color, context);
        }
    }

    void UIRenderer::drawTexture2D(const Texture& texture, glm::vec2 position, glm::vec2 size, glm::vec4 color, const Context& context) const {
        if (!context.renderPass)
            throw std::logic_error("UIRenderer::drawTexture2D called outside render pass");

        context.uiPipeline.bind(context.renderPass);

        struct UIUniforms {
            glm::vec2 screenSize;
            glm::vec2 position;
            glm::vec2 size;
            glm::vec2 padding;
        };

        UIUniforms uiUniforms{};
        uiUniforms.screenSize = {
            static_cast<float>(context.window.width()),
            static_cast<float>(context.window.height())
        };
        uiUniforms.position = position;
        uiUniforms.size = size;

        SDL_PushGPUVertexUniformData(context.commandBuffer, 0, &uiUniforms, sizeof(UIUniforms));

        struct UIColorUniforms {
            glm::vec4 color;
        };

        UIColorUniforms colorUniforms{};
        colorUniforms.color = color;

        SDL_PushGPUFragmentUniformData(context.commandBuffer, 0, &colorUniforms, sizeof(UIColorUniforms));

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = context.quadMesh.vertexBuffer();
        vertexBinding.offset = 0;

        SDL_BindGPUVertexBuffers(context.renderPass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = context.quadMesh.indexBuffer();
        indexBinding.offset = 0;

        SDL_BindGPUIndexBuffer(context.renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_GPUTextureSamplerBinding textureBinding{};
        textureBinding.texture = texture.handle();
        textureBinding.sampler = context.defaultSampler;

        SDL_BindGPUFragmentSamplers(context.renderPass, 0, &textureBinding, 1);

        SDL_DrawGPUIndexedPrimitives(context.renderPass, context.quadMesh.indexCount(), 1, 0, 0, 0);
    }
}
