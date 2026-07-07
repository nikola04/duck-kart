#pragma once

#include <SDL3/SDL_stdinc.h>
#include <cstddef>
#include <glm/vec4.hpp>

namespace engine {
    constexpr std::size_t ShadowCascadeCount = 4;
    constexpr float ShadowCascadeOverlap = 10.0f;

    constexpr float ShadowCascadeSplits[ShadowCascadeCount] = {
        24.0f,
        68.0f,
        170.0f,
        500.0f
    };

    constexpr Uint32 ShadowCascadeTextureSizes[ShadowCascadeCount] = {
        4096,
        4096,
        2048,
        1024
    };

    constexpr glm::vec4 ShadowCascadeSplitsUniform {
        ShadowCascadeSplits[0],
        ShadowCascadeSplits[1],
        ShadowCascadeSplits[2],
        ShadowCascadeSplits[3]
    };
}
