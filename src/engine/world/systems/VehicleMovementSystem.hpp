#pragma once

#include "../../core/Input.hpp"
#include "../World.hpp"

namespace engine {
    class VehicleMovementSystem {
        public:
            void update(World& world, Input& input, float dt) const;
    };
}
