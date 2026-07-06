#pragma once

#include <glm/mat4x4.hpp>

namespace engine {
    struct ShadowLightUniforms {
        glm::mat4 lightView;
        glm::mat4 lightProjection;
        glm::mat4 lightVP;
    };
}
