#include "FreeCameraControllerSystem.hpp"

#include "../components/FreeCameraControllerComponent.hpp"
#include "../components/TransformComponent.hpp"

namespace engine {
    void FreeCameraControllerSystem::update(World& world, Input& input, float dt) {
        for (Entity e : world.entities()) {
            auto* transform = world.get<TransformComponent>(e);
            auto* camera = world.get<FreeCameraControllerComponent>(e);

            if (!transform || !camera) continue;

            // mouse movement
            transform->transform.rotation.y += input.mouseDeltaX() * camera->mouseSensitivity;
            transform->transform.rotation.x -= input.mouseDeltaY() * camera->mouseSensitivity;
            transform->transform.rotation.x = std::clamp(transform->transform.rotation.x, -camera->maxPitch, camera->maxPitch);

            // keyboard movement
            float speed = input.isKeyDown(SDL_SCANCODE_LCTRL) ? camera->speed * 10 : camera->speed;

            if (input.isKeyDown(SDL_SCANCODE_W))
                transform->transform.position += transform->transform.forward() * speed * dt;
            if (input.isKeyDown(SDL_SCANCODE_S))
                transform->transform.position -= transform->transform.forward() * speed * dt;
            if (input.isKeyDown(SDL_SCANCODE_A))
                transform->transform.position -= transform->transform.right() * speed * dt;
            if (input.isKeyDown(SDL_SCANCODE_D))
                transform->transform.position += transform->transform.right() * speed * dt;
        }
    }
}
