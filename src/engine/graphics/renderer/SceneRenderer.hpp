#pragma once

#include "RenderMesh.hpp"
#include "../../math/Transform.hpp"
#include "../../scene/Camera.hpp"
#include "../../scene/Skybox.hpp"
#include "../../scene/Scene.hpp"
#include "../GraphicsPipeline.hpp"
#include "../ShadowCamera.hpp"
#include "../ShadowConstants.hpp"
#include "../ShadowMap.hpp"
#include "../Texture.hpp"
#include "../uniforms/PointLightUniforms.hpp"
#include "../Material.hpp"
#include "../../window/Window.hpp"
#include <array>
#include <cstddef>
#include <optional>

namespace engine {
    struct SceneRenderStats {
        std::size_t visibleChunks = 0;
        std::size_t visibleObjects = 0;
        std::size_t drawCalls = 0;
    };

    class SceneRenderer {
        public:
            struct Context {
                Window& window;
                SDL_GPUCommandBuffer* commandBuffer = nullptr;
                SDL_GPURenderPass* renderPass = nullptr;
                GraphicsPipeline& mainPipeline;
                GraphicsPipeline& skyboxPipeline;
                const RenderMesh& skyboxMesh;
                SDL_GPUSampler* defaultSampler = nullptr;
                SDL_GPUSampler* shadowSampler = nullptr;
                const Texture& whiteTexture;
                const Texture& defaultNormalTexture;
                const std::array<std::optional<ShadowMap>, ShadowCascadeCount>& shadowMaps;
                const std::array<ShadowCamera, ShadowCascadeCount>& shadowCameras;
            };

            SceneRenderStats render(const Scene& scene, const Context& context) const;

        private:
            void draw(
                const Context& context,
                const RenderMesh& mesh,
                const Transform& transform,
                const Camera& camera,
                const Material& material,
                const Skybox& skybox,
                const DirectionalLight& light,
                const PointLightUniforms& pointLights
            ) const;
            void drawSkybox(const Context& context, const Skybox& skybox, const Camera& camera) const;
    };
}
