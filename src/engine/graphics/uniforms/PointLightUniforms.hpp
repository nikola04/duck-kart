#pragma once

#include "../../scene/Scene.hpp"

#include <array>
#include <cstdint>
#include <glm/vec4.hpp>

namespace engine {
    constexpr std::uint32_t MaxPointLights = 8;

    struct PointLightUniforms {
        glm::vec4 count{0.0f, 0.0f, 0.0f, 0.0f};
        std::array<PointLight, MaxPointLights> lights{};
    };
}
