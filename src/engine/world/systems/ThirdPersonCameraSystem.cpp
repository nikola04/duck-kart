#include "ThirdPersonCameraSystem.hpp"

#include "../components/ThirdPersonCameraComponent.hpp"
#include "../components/TransformComponent.hpp"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace engine {
    static float wrapRadians(float angle) {
        constexpr float pi = 3.14159265358979323846f;
        constexpr float twoPi = pi * 2.0f;

        while (angle > pi)
            angle -= twoPi;

        while (angle < -pi)
            angle += twoPi;

        return angle;
    }

    void ThirdPersonCameraSystem::update(World& world, float dt) const {
        for (Entity entity : world.entities()) {
            auto* cameraTransform = world.get<TransformComponent>(entity);
            auto* camera = world.get<ThirdPersonCameraComponent>(entity);

            if (!cameraTransform || !camera || camera->target == NullEntity)
                continue;

            const auto* targetTransform = world.get<TransformComponent>(camera->target);
            if (!targetTransform)
                continue;

            const glm::vec3 targetPosition = targetTransform->transform.position;
            const float targetYaw = targetTransform->transform.rotation.y;

            if (!camera->initialized) {
                camera->currentYaw = targetYaw;
                camera->initialized = true;
            }

            const float yawDelta = wrapRadians(targetYaw - camera->currentYaw);
            const float follow = 1.0f - std::exp(-camera->followSharpness * dt);
            camera->currentYaw += yawDelta * follow;

            const glm::vec3 cameraForward{
                std::sin(camera->currentYaw),
                0.0f,
                -std::cos(camera->currentYaw)
            };

            cameraTransform->transform.position =
                targetPosition - cameraForward * camera->distance + glm::vec3(0.0f, camera->height, 0.0f);

            const glm::vec3 lookAt = targetPosition + glm::vec3(0.0f, camera->lookAtHeight, 0.0f);
            const glm::vec3 direction = glm::normalize(lookAt - cameraTransform->transform.position);

            cameraTransform->transform.rotation.x = std::asin(direction.y);
            cameraTransform->transform.rotation.y = std::atan2(direction.x, -direction.z);
            cameraTransform->transform.rotation.z = 0.0f;
        }
    }
}
