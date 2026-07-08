#pragma once

#include "RenderMesh.hpp"
#include "../../scene/Scene.hpp"
#include "../GraphicsPipeline.hpp"
#include "../ShadowCamera.hpp"
#include "../ShadowConstants.hpp"
#include "../Texture.hpp"
#include <array>
#include <cstddef>

namespace engine {
    class ShadowRenderer {
        public:
            struct Context {
                SDL_GPUCommandBuffer* commandBuffer = nullptr;
                SDL_GPURenderPass* renderPass = nullptr;
                GraphicsPipeline& shadowPipeline;
                SDL_GPUSampler* defaultSampler = nullptr;
                const Texture& whiteTexture;
                const std::array<ShadowCamera, ShadowCascadeCount>& shadowCameras;
            };

            void render(const Scene& scene, const Context& context, std::size_t cascade) const;
            void drawShadow(const Context& context, const RenderMesh& mesh, const Transform& transform, const Material& material, std::size_t cascade) const;
    };
}
