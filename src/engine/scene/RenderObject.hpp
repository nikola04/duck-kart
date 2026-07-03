#pragma once

#include "../graphics/RenderMesh.hpp"
#include "../graphics/Texture.hpp"
#include "../math/Transform.hpp"

namespace engine {
    struct RenderObject {
        const RenderMesh* mesh = nullptr;
        const Texture* texture = nullptr;
        Transform transform;
    };
}
