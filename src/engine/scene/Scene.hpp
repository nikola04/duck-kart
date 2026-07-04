#pragma once

#include "Camera.hpp"
#include "../graphics/uniforms/LightUniforms.hpp"
#include "RenderObject.hpp"
#include <vector>

namespace engine {
    struct Scene {
        Camera camera;
        LightUniforms sun{
             .direction = {-0.4f, -1.0f, -0.3f, 0.0f},
             .color = {1.0f, 1.0f, 0.95f, 1.0f},
             .ambient = {0.15f, 0.15f, 0.18f, 1.0f}
         };

        std::vector<RenderObject> objects;

        void clear() {
            objects.clear();
        }
    };
}
