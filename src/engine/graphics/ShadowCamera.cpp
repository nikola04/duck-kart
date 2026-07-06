#include "ShadowCamera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace engine {
    void ShadowCamera::update(const DirectionalLight& light, const glm::vec3& target) {
        glm::vec3 dir = glm::normalize(glm::vec3(light.direction));
        constexpr float shadowHalfExtent = 300.0f;
        constexpr float lightDistance = 100.0f;
        constexpr float shadowFarPlane = 300.0f;

        glm::vec3 eye = target - dir * lightDistance;

        glm::vec3 up{0.0f, 1.0f, 0.0f};
        if (glm::abs(glm::dot(dir, up)) > 0.99f)
            up = {0.0f, 0.0f, 1.0f};

        m_view = glm::lookAtRH(eye, target, up);

        // SDL GPU uses a 0..1 clip-space depth range. glm::ortho() defaults to
        // OpenGL's -1..1 range, which clipped half of the shadow casters and
        // made the light frustum show up as a dark rectangle in the scene.
        m_projection = glm::orthoRH_ZO(
            -shadowHalfExtent,
            shadowHalfExtent,
            -shadowHalfExtent,
            shadowHalfExtent,
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
