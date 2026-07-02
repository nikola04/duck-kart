#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace engine {
    struct VertexUniforms {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
}
