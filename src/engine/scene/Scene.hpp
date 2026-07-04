#pragma once

#include "Camera.hpp"
#include "RenderObject.hpp"
#include <iterator>
#include <vector>

namespace engine {
    struct RenderModel {
        std::vector<RenderObject> objects;
    };

    struct DirectionalLight {
        glm::vec4 direction{-0.4f, -1.0f, -0.3f, 0.0f};
        glm::vec4 color{1.0f, 1.0f, 0.95f, 1.0f};
        glm::vec4 ambient{0.15f, 0.15f, 0.18f, 1.0f};
    };

    struct PointLight {
        glm::vec4 position{0.0f, 2.0f, 0.0f, 1.0f};
        glm::vec4 color{1.0f, 0.85f, 0.55f, 1.0f};
        glm::vec4 properties{4.0f, 8.0f, 0.0f, 0.0f}; // x = intensity, y = radius
    };

    struct Scene {
        Camera camera;
        DirectionalLight sun;

        std::vector<RenderObject> objects;
        std::vector<PointLight> pointLights;

        void clear() {
            objects.clear();
        }

        void addModel(RenderModel&& model) {
            objects.insert(objects.end(), std::make_move_iterator(model.objects.begin()), std::make_move_iterator(model.objects.end()));
        }
    };
}
