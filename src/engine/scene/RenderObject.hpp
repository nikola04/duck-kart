#pragma once

#include "../graphics/RenderMesh.hpp"
#include "../graphics/Texture.hpp"
#include "../graphics/Material.hpp"
#include "../math/Transform.hpp"

namespace engine {
    struct RenderObject {
        const RenderMesh* mesh = nullptr;
        const Texture* texture = nullptr;

        glm::vec4 baseColor{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic = 0.0f;
        float roughness = 1.0f;
        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.5f;

        Transform transform;
    };
}
