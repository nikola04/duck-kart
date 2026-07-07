#include "Frustum.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>

namespace engine {
    static Plane makePlane(const glm::vec4& p) {
        glm::vec3 normal{p.x, p.y, p.z};
        float length = glm::length(normal);

        return Plane {
            .normal = normal / length,
            .distance = p.w / length
        };
    }

    void Frustum::update(const glm::mat4& m) {
        glm::vec4 row0{m[0][0], m[1][0], m[2][0], m[3][0]};
        glm::vec4 row1{m[0][1], m[1][1], m[2][1], m[3][1]};
        glm::vec4 row2{m[0][2], m[1][2], m[2][2], m[3][2]};
        glm::vec4 row3{m[0][3], m[1][3], m[2][3], m[3][3]};


        m_planes[0] = makePlane(row3 + row0); // left
        m_planes[1] = makePlane(row3 - row0); // right
        m_planes[2] = makePlane(row3 + row1); // bottom
        m_planes[3] = makePlane(row3 - row1); // top

        m_planes[4] = makePlane(row2);
        m_planes[5] = makePlane(row3 - row2);
    }

    bool Frustum::intersects(const AABB& aabb) const {
        for (const auto& plane : m_planes) {
            glm::vec3 positive = aabb.min;

            if (plane.normal.x >= 0.0f) positive.x = aabb.max.x;
            if (plane.normal.y >= 0.0f) positive.y = aabb.max.y;
            if (plane.normal.z >= 0.0f) positive.z = aabb.max.z;

            if (plane.signedDistance(positive) < 0.0f)
                return false;
        }
        return true;
    }
}
