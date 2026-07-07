#pragma once

#include "../graphics/renderer/RenderMesh.hpp"
#include "../graphics/Material.hpp"
#include "../math/Transform.hpp"

namespace engine {
    struct RenderObject {
        const RenderMesh* mesh = nullptr;
        const Material* material = nullptr;

        Transform transform;
    };
}
