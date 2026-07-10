#pragma once

#include "../World.hpp"

namespace engine {
    struct ThirdPersonCameraComponent {
        Entity target = NullEntity;
        float distance = 8.0f;
        float height = 4.0f;
        float lookAtHeight = 1.0f;
        float followSharpness = 4.0f;
        float currentYaw = 0.0f;
        bool initialized = false;
    };
}
