#include "VehicleMovementSystem.hpp"

#include "../components/TransformComponent.hpp"
#include "../components/VehicleMovementComponent.hpp"

#include <SDL3/SDL_scancode.h>

namespace engine {
    void VehicleMovementSystem::update(World& world, Input& input, float dt) const {
        for (Entity entity : world.entities()) {
            auto* transform = world.get<TransformComponent>(entity);
            auto* vehicle = world.get<VehicleMovementComponent>(entity);

            if (!transform || !vehicle)
                continue;

            if (input.isKeyDown(SDL_SCANCODE_A))
                transform->transform.rotation.y += vehicle->turnSpeed * dt;

            if (input.isKeyDown(SDL_SCANCODE_D))
                transform->transform.rotation.y -= vehicle->turnSpeed * dt;

            if (input.isKeyDown(SDL_SCANCODE_W))
                transform->transform.position += transform->transform.forward() * vehicle->forwardSpeed * dt;

            if (input.isKeyDown(SDL_SCANCODE_S))
                transform->transform.position -= transform->transform.forward() * vehicle->reverseSpeed * dt;
        }
    }
}
