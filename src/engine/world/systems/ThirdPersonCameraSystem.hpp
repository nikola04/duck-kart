#pragma once

#include "../World.hpp"

namespace engine {
    class ThirdPersonCameraSystem {
        public:
            void update(World& world, float dt) const;
    };
}
