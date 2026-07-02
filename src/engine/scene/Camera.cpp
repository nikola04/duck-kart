#include "Camera.hpp"

namespace engine {
    glm::mat4 Camera::viewMatrix() const {
        return glm::lookAt(transform.position, transform.position + transform.forward(), transform.up());
    }

    glm::mat4 Camera::projectionMatrix(float aspect_ration) const {
        return glm::perspective(glm::radians(fov), aspect_ration, near_plane, far_plane);
    }
}
