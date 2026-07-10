#pragma once

#include "../World.hpp"
#include "../../core/Input.hpp"

namespace engine {
    class FreeCameraControllerSystem {
        public:
            void update(World& world, Input& input, float dt);
    };
}
