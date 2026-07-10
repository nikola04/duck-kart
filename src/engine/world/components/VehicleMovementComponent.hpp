#pragma once

#include <glm/trigonometric.hpp>

namespace engine {
    struct VehicleMovementComponent {
        float forwardSpeed = 12.0f;
        float reverseSpeed = 5.0f;
        float turnSpeed = glm::radians(120.0f);
    };
}
