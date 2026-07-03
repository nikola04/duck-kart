#pragma once

#include <glm/vec4.hpp>

namespace engine
{
    struct LightUniforms {
        glm::vec4 direction{-0.4f, -1.0f, -0.3f, 0.0f};
        glm::vec4 color{1.0f, 1.0f, 0.95f, 1.0f};
        glm::vec4 ambient{0.15f, 0.15f, 0.18f, 1.0f};
    };
}
