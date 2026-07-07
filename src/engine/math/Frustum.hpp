#pragma once

#include "AABB.hpp"
#include <cfloat>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace engine {
    struct Plane {
        glm::vec3 normal;
        float distance;

        float signedDistance(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    class Frustum{
        public:
            void update(const glm::mat4& viewProjection);
            bool intersects(const AABB&) const;

        private:
            Plane m_planes[6];
    };
}
