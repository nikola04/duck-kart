#pragma once

#include "Camera.hpp"
#include "RenderObject.hpp"
#include "Skybox.hpp"
#include <iterator>
#include <vector>

namespace engine {
    struct RenderModel {
        std::vector<RenderObject> objects;
    };

    struct DirectionalLight {
        glm::vec4 direction{0.05f, -0.25f, 0.2f, 0.0f};
        glm::vec4 color{1.0f, 0.98f, 0.92f, 5.0f};
        glm::vec4 ambient{0.08f, 0.08f, 0.09f, 1.0f};
        glm::vec4 properties{1.5f, 1.1f, 0.0f, 0.0f};
    };

    struct PointLight {
        glm::vec4 position{0.0f, 2.0f, 0.0f, 1.0f};
        glm::vec4 color{1.0f, 0.85f, 0.55f, 1.0f};
        glm::vec4 properties{4.0f, 8.0f, 0.0f, 0.0f}; // x = intensity, y = radius
    };

    struct UITexture {
        const Texture* texture{};
        glm::vec2 position{};
        glm::vec2 size{};
        glm::vec4 color{1, 1, 1, 1};
    };

    struct Scene {
        Camera camera;
        Skybox skybox;
        DirectionalLight sun;

        std::vector<RenderObject> objects;
        std::vector<PointLight> pointLights;

        void clear() {
            objects.clear();
            uiTextures.clear();
        }

        void addModel(RenderModel&& model) {
            objects.insert(objects.end(), std::make_move_iterator(model.objects.begin()), std::make_move_iterator(model.objects.end()));
        }

        std::vector<UITexture> uiTextures;
    };
}
