#pragma once

#include <glm/mat4x4.hpp>

namespace engine {
    struct ShadowVertexUniforms {
        glm::mat4 model;
        glm::mat4 lightView;
        glm::mat4 lightProjection;
    };
}
