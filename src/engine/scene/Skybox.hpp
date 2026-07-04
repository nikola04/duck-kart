#pragma once

#include "../graphics/Cubemap.hpp"

namespace engine {
    class Skybox {
        public:
            const Cubemap* cubemap = nullptr;
    };
}
