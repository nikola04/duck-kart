#include "SceneRenderer.hpp"
#include "../../math/Frustum.hpp"
#include "../uniforms/CameraUniforms.hpp"
#include "../uniforms/MaterialUniforms.hpp"
#include "../uniforms/ShadowLightUniforms.hpp"
#include "../uniforms/VertexUniforms.hpp"
#include <algorithm>
#include <stdexcept>

namespace engine {
    SceneRenderStats SceneRenderer::render(const Scene& scene, const Context& context) const {
        SceneRenderStats stats{};

        PointLightUniforms pointLightUniforms{};
        std::size_t count = std::min(scene.pointLights.size(), static_cast<std::size_t>(MaxPointLights));
        pointLightUniforms.count.x = static_cast<float>(count);

        for (std::size_t i = 0; i < count; ++i)
            pointLightUniforms.lights[i] = scene.pointLights[i];

        const float aspectRatio =
            static_cast<float>(context.window.width()) /
            static_cast<float>(context.window.height());

        Frustum cameraFrustum;
        cameraFrustum.update(scene.camera.projectionMatrix(aspectRatio) * scene.camera.viewMatrix());

        for (const auto& [coords, chunk] : scene.chunks) {
            if (!cameraFrustum.intersects(chunk.bounds))
                continue;

            stats.visibleChunks++;

            for (const auto& object : chunk.objects) {
                draw(context, *object.mesh, object.transform, scene.camera, *object.material, scene.skybox, scene.sun, pointLightUniforms);
                stats.visibleObjects++;
                stats.drawCalls++;
            }
        }

        drawSkybox(context, scene.skybox, scene.camera);
        stats.drawCalls++;

        return stats;
    }

    void SceneRenderer::drawSkybox(const Context& context, const Skybox& skybox, const Camera& camera) const {
        if (!context.renderPass)
            throw std::logic_error("SceneRenderer::drawSkybox called outside an active render pass");

        if (!skybox.cubemap)
            return;

        context.skyboxPipeline.bind(context.renderPass);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = context.skyboxMesh.vertexBuffer();
        vertexBinding.offset = 0;

        SDL_BindGPUVertexBuffers(context.renderPass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = context.skyboxMesh.indexBuffer();
        indexBinding.offset = 0;

        SDL_BindGPUIndexBuffer(context.renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        float aspectRatio =
            static_cast<float>(context.window.width()) /
            static_cast<float>(context.window.height());

        struct SkyboxUniforms {
            glm::mat4 view;
            glm::mat4 projection;
        };

        SkyboxUniforms uniforms{};
        uniforms.view = camera.viewMatrix();
        uniforms.projection = camera.projectionMatrix(aspectRatio);

        SDL_PushGPUVertexUniformData(context.commandBuffer, 0, &uniforms, sizeof(uniforms));

        SDL_GPUTextureSamplerBinding binding{};
        binding.texture = skybox.cubemap->handle();
        binding.sampler = context.defaultSampler;

        SDL_BindGPUFragmentSamplers(context.renderPass, 0, &binding, 1);

        SDL_DrawGPUIndexedPrimitives(context.renderPass, context.skyboxMesh.indexCount(), 1, 0, 0, 0);
    }

    void SceneRenderer::draw(
        const Context& context,
        const RenderMesh& mesh,
        const Transform& transform,
        const Camera& camera,
        const Material& material,
        const Skybox& skybox,
        const DirectionalLight& light,
        const PointLightUniforms& pointLights
    ) const {
        if (!context.renderPass)
            throw std::logic_error("SceneRenderer::draw called outside an active render pass");

        if (!skybox.cubemap)
            throw std::logic_error("SceneRenderer::draw requires a skybox cubemap for scene lighting");

        float aspectRatio = static_cast<float>(context.window.width()) / static_cast<float>(context.window.height());

        VertexUniforms uniforms{};
        uniforms.model = transform.matrix();
        uniforms.view = camera.viewMatrix();
        uniforms.projection = camera.projectionMatrix(aspectRatio);
        SDL_PushGPUVertexUniformData(context.commandBuffer, 0, &uniforms, sizeof(VertexUniforms));

        ShadowLightUniforms shadowUniforms{};
        for (std::size_t i = 0; i < ShadowCascadeCount; ++i)
            shadowUniforms.lightVP[i] = context.shadowCameras[i].projection() * context.shadowCameras[i].view();

        SDL_PushGPUVertexUniformData(context.commandBuffer, 1, &shadowUniforms, sizeof(ShadowLightUniforms));

        MaterialUniforms materialUniforms{};
        materialUniforms.base_color = material.baseColor;
        materialUniforms.properties = { material.metallic, material.roughness, material.alphaCutoff, material.alphaMode };
        SDL_PushGPUFragmentUniformData(context.commandBuffer, 0, &materialUniforms, sizeof(MaterialUniforms));

        CameraUniforms cameraUniforms{};
        cameraUniforms.position = glm::vec4{camera.transform.position, 1.0f};
        cameraUniforms.cascadeSplits = ShadowCascadeSplitsUniform;
        SDL_PushGPUFragmentUniformData(context.commandBuffer, 1, &cameraUniforms, sizeof(CameraUniforms));
        SDL_PushGPUFragmentUniformData(context.commandBuffer, 2, &light, sizeof(DirectionalLight));
        SDL_PushGPUFragmentUniformData(context.commandBuffer, 3, &pointLights, sizeof(PointLightUniforms));

        context.mainPipeline.bind(context.renderPass);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = mesh.vertexBuffer();
        vertexBinding.offset = 0;

        SDL_BindGPUVertexBuffers(context.renderPass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = mesh.indexBuffer();
        indexBinding.offset = 0;

        SDL_BindGPUIndexBuffer(context.renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        const Texture* textureToBind = material.texture ? material.texture : &context.whiteTexture;
        const Texture* normalTextureToBind = material.normalTexture ? material.normalTexture : &context.defaultNormalTexture;

        constexpr std::size_t bindingsCount = 3 + ShadowCascadeCount;
        SDL_GPUTextureSamplerBinding bindings[bindingsCount] = {
            { .texture = textureToBind->handle(), .sampler = context.defaultSampler },
            { .texture = normalTextureToBind->handle(), .sampler = context.defaultSampler },
            { .texture = skybox.cubemap->handle(), .sampler = context.defaultSampler },
        };

        std::size_t offset = bindingsCount - ShadowCascadeCount;
        for (std::size_t i = 0; i < ShadowCascadeCount; i++)
            bindings[offset + i] = { .texture = context.shadowMaps[i]->texture(), .sampler = context.shadowSampler };

        SDL_BindGPUFragmentSamplers(context.renderPass, 0, bindings, bindingsCount);

        SDL_DrawGPUIndexedPrimitives(context.renderPass, mesh.indexCount(), 1, 0, 0, 0);
    }
}
