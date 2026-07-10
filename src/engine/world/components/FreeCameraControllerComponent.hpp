#pragma once

#include <glm/trigonometric.hpp>
namespace engine {
    struct FreeCameraControllerComponent {
          float speed = 3.0f;
          float sprintSpeed = 30.0f;
          float mouseSensitivity = 0.0025f;
          float maxPitch = glm::radians(89.0f);
    };
}
