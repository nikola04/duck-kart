#pragma once

#include "../graphics/RenderMesh.hpp"
#include "../graphics/Material.hpp"
#include "../math/Transform.hpp"

namespace engine {
    struct RenderObject {
        const RenderMesh* mesh = nullptr;
        const Material* material = nullptr;

        Transform transform;
    };
}
