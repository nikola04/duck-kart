#pragma once

#include "../graphics/RenderMesh.hpp"
#include "../math/Transform.hpp"

namespace engine {
    struct RenderObject {
        const RenderMesh* mesh = nullptr;
        Transform transform;
    };
}
