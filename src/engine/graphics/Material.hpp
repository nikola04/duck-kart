#pragma once

#include "Texture.hpp"
#include <glm/vec4.hpp>

namespace engine {
    enum class AlphaMode {
        Opaque  = 0,
        Mask    = 1,
        Blend   = 2
    };

    struct Material {
        glm::vec4 baseColor{1.0f};

        const Texture* texture = nullptr;
        const Texture* normalTexture = nullptr;

        float metallic = 0.0f;
        float roughness = 1.0f;

        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.5f;
    };
}
