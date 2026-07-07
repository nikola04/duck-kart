#pragma once

#include "RenderMesh.hpp"
#include "../../scene/Scene.hpp"
#include "../GraphicsPipeline.hpp"
#include "../ShadowCamera.hpp"
#include "../ShadowConstants.hpp"
#include <array>
#include <cstddef>

namespace engine {
    class ShadowRenderer {
        public:
            struct Context {
                SDL_GPUCommandBuffer* commandBuffer = nullptr;
                SDL_GPURenderPass* renderPass = nullptr;
                GraphicsPipeline& shadowPipeline;
                const std::array<ShadowCamera, ShadowCascadeCount>& shadowCameras;
            };

            void render(const Scene& scene, const Context& context, std::size_t cascade) const;
            void drawShadow(const Context& context, const RenderMesh& mesh, const Transform& transform, std::size_t cascade) const;
    };
}
