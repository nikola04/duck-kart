#pragma once

#include "RenderMesh.hpp"
#include "../../scene/Scene.hpp"
#include "../../window/Window.hpp"
#include "../GraphicsPipeline.hpp"
#include "../Texture.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace engine {
    class UIRenderer {
        public:
            struct Context {
                Window& window;
                SDL_GPUCommandBuffer* commandBuffer = nullptr;
                SDL_GPURenderPass* renderPass = nullptr;
                GraphicsPipeline& uiPipeline;
                const RenderMesh& quadMesh;
                SDL_GPUSampler* defaultSampler = nullptr;
            };

            void render(const std::vector<UITexture>& textures, const Context& context) const;
            void drawTexture2D(const Texture& texture, glm::vec2 position, glm::vec2 size, glm::vec4 color, const Context& context) const;
    };
}
