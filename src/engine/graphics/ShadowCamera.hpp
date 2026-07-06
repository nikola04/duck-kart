#pragma once

#include "../scene/Scene.hpp"
#include <glm/glm.hpp>

namespace engine {
    class ShadowCamera {
        public:
            void update(
                const DirectionalLight& light,
                const glm::vec3& target,
                float halfExtent,
                float shadowMapSize
            );

            void updateFromFrustumSlice(
                const DirectionalLight& light,
                const Camera& camera,
                float nearPlane,
                float farPlane,
                float aspect,
                float shadowMapSize
            );

            const glm::mat4& view() const;
            const glm::mat4& projection() const;

        private:
            glm::mat4 m_view{1.0f};
            glm::mat4 m_projection{1.0f};
        };
}
