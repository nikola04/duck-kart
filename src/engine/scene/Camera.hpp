#pragma once

#include "../math/Transform.hpp"

namespace engine {
    class Camera {
        public:
            Transform transform;

            float fov = 70.0f;
            float near_plane = 0.25f;
            float far_plane = 2048.0f;

            glm::mat4 viewMatrix() const;
            glm::mat4 projectionMatrix(float aspect_ration) const;
    };
}
