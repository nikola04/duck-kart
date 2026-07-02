#include "Transform.hpp"

namespace engine {
    glm::mat4 Transform::matrix() const {
        glm::mat4 result{1.0f};

        result = glm::translate(result, position);

        result = glm::rotate(result, rotation.x, glm::vec3{ 1.0f, 0.0f, 0.0f });
        result = glm::rotate(result, rotation.y, glm::vec3{ 0.0f, 1.0f, 0.0f });
        result = glm::rotate(result, rotation.z, glm::vec3{ 0.0f, 0.0f, 1.0f });

        result = glm::scale(result, scale);

        return result;
    }

    glm::vec3 Transform::forward() const {
        const float pitch = rotation.x;
        const float yaw = rotation.y;

        glm::vec3 direction;
        direction.x = std::sin(yaw) * std::cos(pitch);
        direction.y = std::sin(pitch);
        direction.z = -std::cos(yaw) * std::cos(pitch);

        return glm::normalize(direction);
    }

    glm::vec3 Transform::right() const {
        return glm::normalize(glm::cross(forward(), glm::vec3{0.0f, 1.0f, 0.0f}));
    }

    glm::vec3 Transform::up() const {
        return glm::normalize(glm::cross(right(), forward()));
    }
}
