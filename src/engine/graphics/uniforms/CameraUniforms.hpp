#pragma once

#include <glm/vec4.hpp>

namespace engine
{
    struct CameraUniforms {
        glm::vec4 position;
        glm::vec4 cascadeSplits;
    };
}
