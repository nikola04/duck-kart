#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <array>
#include <cfloat>

namespace engine {
    struct AABB {
        glm::vec3 min{ FLT_MAX };
        glm::vec3 max{ -FLT_MAX };

        void expand(const glm::vec3& p) {
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        AABB transformed(const glm::mat4& matrix) const {
            std::array<glm::vec3, 8> corners = {
                glm::vec3{min.x, min.y, min.z},
                glm::vec3{max.x, min.y, min.z},
                glm::vec3{min.x, max.y, min.z},
                glm::vec3{max.x, max.y, min.z},
                glm::vec3{min.x, min.y, max.z},
                glm::vec3{max.x, min.y, max.z},
                glm::vec3{min.x, max.y, max.z},
                glm::vec3{max.x, max.y, max.z},
            };

            AABB result;

            for (const auto& corner : corners) {
                glm::vec4 p = matrix * glm::vec4(corner, 1.0f);
                result.expand(glm::vec3(p));
            }

            return result;
        }
    };
}
