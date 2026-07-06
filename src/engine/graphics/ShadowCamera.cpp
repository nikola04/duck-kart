#include "ShadowCamera.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

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

        m_projection = glm::orthoRH_ZO(
            -halfExtent,
            halfExtent,
            -halfExtent,
            halfExtent,
            0.1f,
            shadowFarPlane
        );
    }

    void ShadowCamera::updateFromFrustumSlice(
        const DirectionalLight& light,
        const Camera& camera,
        float nearPlane,
        float farPlane,
        float aspect,
        float shadowMapSize
    ) {
        glm::mat4 view = camera.viewMatrix();
        glm::mat4 projection = glm::perspectiveRH_ZO(glm::radians(camera.fov), aspect, nearPlane, farPlane);

        glm::mat4 invViewProj = glm::inverse(projection * view);

        std::array<glm::vec3, 8> corners;
        std::size_t index = 0;

        for (float x : { -1.0f, 1.0f }) {
            for (float y : { -1.0f, 1.0f }) {
                for (float z : { 0.0f, 1.0f }) {
                    glm::vec4 corner = invViewProj * glm::vec4(x, y, z, 1.0f);
                    corner /= corner.w;
                    corners[index++] = glm::vec3(corner);
                }
            }
        }

        glm::vec3 center{0.0f};
        for (const auto& corner : corners)
            center += corner;
        center /= static_cast<float>(corners.size());

        glm::vec3 lightDir = glm::normalize(glm::vec3(light.direction));

        float radius = 0.0f;
        for (const auto& corner : corners)
            radius = std::max(radius, glm::length(corner - center));
        radius = std::ceil(radius);

        glm::vec3 eye = center - lightDir * radius * 2.0f;
        glm::vec3 up{0.0f, 1.0f, 0.0f};
        if(glm::abs(glm::dot(lightDir, up)) > 0.99f)
            up = {0.0f, 0.0f, 1.0f};

        glm::mat4 lightView = glm::lookAtRH(eye, center, up);

        float minX = FLT_MAX, maxX = -FLT_MAX, minY =  FLT_MAX, maxY = -FLT_MAX, minZ =  FLT_MAX, maxZ = -FLT_MAX;

        for (const auto& corner : corners) {
            glm::vec4 lightSpaceCorner = lightView * glm::vec4(corner, 1.0f);

            minX = std::min(minX, lightSpaceCorner.x);
            maxX = std::max(maxX, lightSpaceCorner.x);
            minY = std::min(minY, lightSpaceCorner.y);
            maxY = std::max(maxY, lightSpaceCorner.y);
            minZ = std::min(minZ, lightSpaceCorner.z);
            maxZ = std::max(maxZ, lightSpaceCorner.z);
        }

        float worldUnitsPerTexel = (maxX - minX) / shadowMapSize;

        minX = std::floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
        maxX = std::floor(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
        minY = std::floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;
        maxY = std::floor(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

        constexpr float zMultiplier = 10.0f;

        if (minZ < 0.0f)
            minZ *= zMultiplier;
        else minZ /= zMultiplier;

        if (maxZ < 0.0f)
            maxZ /= zMultiplier;
        else maxZ *= zMultiplier;

        m_view = lightView;

        m_projection = glm::orthoRH_ZO(minX, maxX, minY, maxY, -maxZ, -minZ);
    }

    const glm::mat4& ShadowCamera::view() const {
        return m_view;
    }

    const glm::mat4& ShadowCamera::projection() const {
        return m_projection;
    }
}
