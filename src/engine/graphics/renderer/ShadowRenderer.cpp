#include "ShadowRenderer.hpp"
#include "../../math/Frustum.hpp"
#include "../uniforms/ShadowVertexUniforms.hpp"
#include <stdexcept>

namespace engine {
    void ShadowRenderer::render(const Scene& scene, const Context& context, std::size_t cascade) const {
        Frustum shadowFrustum;
        shadowFrustum.update(context.shadowCameras[cascade].projection() * context.shadowCameras[cascade].view());

        for (const auto& [coords, chunk] : scene.chunks) {
            if (!shadowFrustum.intersects(chunk.bounds))
                continue;

            for (const auto& object : chunk.objects) {
                if (!shadowFrustum.intersects(object.bounds))
                    continue;

                drawShadow(context, *object.mesh, object.transform, cascade);
            }
        }
    }

    void ShadowRenderer::drawShadow(const Context& context, const RenderMesh& mesh, const Transform& transform, std::size_t cascade) const {
        if (!context.renderPass)
            throw std::logic_error("ShadowRenderer::drawShadow called outside an active shadow pass");

        ShadowVertexUniforms uniforms{};
        uniforms.model = transform.matrix();
        uniforms.lightView = context.shadowCameras[cascade].view();
        uniforms.lightProjection = context.shadowCameras[cascade].projection();

        SDL_PushGPUVertexUniformData(context.commandBuffer, 0, &uniforms, sizeof(ShadowVertexUniforms));

        context.shadowPipeline.bind(context.renderPass);

        SDL_GPUBufferBinding vertexBinding{};
        vertexBinding.buffer = mesh.vertexBuffer();
        vertexBinding.offset = 0;

        SDL_BindGPUVertexBuffers(context.renderPass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = mesh.indexBuffer();
        indexBinding.offset = 0;

        SDL_BindGPUIndexBuffer(context.renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_DrawGPUIndexedPrimitives(context.renderPass, mesh.indexCount(), 1, 0, 0, 0);
    }
}
