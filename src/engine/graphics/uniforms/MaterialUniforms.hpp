#pragma once

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

namespace engine
{
    struct MaterialUniforms {
        glm::vec4 base_color{1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 properties{0.0f, 1.0f, 0.0f, 0.0f};
    };
}
