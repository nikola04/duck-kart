#pragma once

#include <glm/mat4x4.hpp>
#include "../ShadowConstants.hpp"

namespace engine {
    struct ShadowLightUniforms {
        glm::mat4 lightVP[ShadowCascadeCount];
        glm::vec4 cascadeSplits;
    };
}
