#pragma once

#include "../World.hpp"

namespace engine {
    class PhysicsSystem {
        public:
            bool snapEntityToTerrain(World& world, Entity entity, float heightOffset, float rayStartOffset, float maxDistance) const;
    };
}
