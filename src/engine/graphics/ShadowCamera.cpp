#include "ShadowCamera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace engine {
    void ShadowCamera::update(
        const DirectionalLight& light,
        const glm::vec3& target,
        float halfExtent,
        float shadowMapSize
    ) {
        glm::vec3 dir = glm::normalize(glm::vec3(light.direction));
        float lightDistance = halfExtent * 2.0f;
        float shadowFarPlane = halfExtent * 4.0f;

        float worldUnitsPerTexel = (halfExtent * 2.0f) / shadowMapSize;

        glm::vec3 eye = target - dir * lightDistance;

        glm::vec3 up{0.0f, 1.0f, 0.0f};
        if (glm::abs(glm::dot(dir, up)) > 0.99f)
            up = {0.0f, 0.0f, 1.0f};

        glm::mat4 tempView = glm::lookAtRH(eye, target, up);

        glm::vec4 targetLS = tempView * glm::vec4(target, 1.0f);

        targetLS.x = std::floor(targetLS.x / worldUnitsPerTexel) * worldUnitsPerTexel;
        targetLS.y = std::floor(targetLS.y / worldUnitsPerTexel) * worldUnitsPerTexel;

        glm::vec3 snappedTargetWS = glm::vec3(glm::inverse(tempView) * targetLS);
        glm::vec3 snappedEye = snappedTargetWS - dir * lightDistance;

        m_view = glm::lookAtRH(snappedEye, snappedTargetWS, up);

        // SDL GPU uses a 0..1 clip-space depth range. glm::ortho() defaults to
        // OpenGL's -1..1 range, which clipped half of the shadow casters and
        // made the light frustum show up as a dark rectangle in the scene.
        m_projection = glm::orthoRH_ZO(
            -halfExtent,
            halfExtent,
            -halfExtent,
            halfExtent,
            0.1f,
            shadowFarPlane
        );
    }

    const glm::mat4& ShadowCamera::view() const {
        return m_view;
    }

    const glm::mat4& ShadowCamera::projection() const {
        return m_projection;
    }
}
