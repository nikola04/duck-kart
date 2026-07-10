#include "PhysicsSystem.hpp"
#include "../components/TerrainColliderComponent.hpp"
#include "../components/TransformComponent.hpp"
#include "../../physics/TerrainCollider.hpp"
#include <chrono>
#include <glm/vec3.hpp>
#include <iostream>

namespace engine {
    bool PhysicsSystem::snapEntityToTerrain(World& world, Entity entity, float heightOffset, float rayStartOffset, float maxDistance) const {
        using Clock = std::chrono::steady_clock;
        static int frameCounter = 0;
        static double accumulatedMs = 0.0;
        static std::size_t accumulatedTestedTriangles = 0;
        static std::size_t accumulatedCandidateTriangles = 0;
        static int hitCount = 0;
        static int gridCount = 0;

        const auto start = Clock::now();

        auto* transform = world.get<TransformComponent>(entity);
        if (!transform)
            return false;

        const TerrainCollider* terrain = nullptr;
        for (Entity candidate : world.entities()) {
            auto* terrainComponent = world.get<TerrainColliderComponent>(candidate);
            if (!terrainComponent)
                continue;

            terrain = &terrainComponent->collider;
            break;
        }

        if (!terrain)
            return false;

        RaycastHit hit;
        TerrainRaycastStats raycastStats;
        const glm::vec3 origin = transform->transform.position + glm::vec3(0.0f, rayStartOffset, 0.0f);
        const glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

        const bool hitTerrain = terrain->raycast(origin, direction, maxDistance, hit, raycastStats);

        const auto end = Clock::now();
        const double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();
        accumulatedMs += elapsedMs;
        accumulatedTestedTriangles += raycastStats.testedTriangles;
        accumulatedCandidateTriangles += raycastStats.candidateTriangles;
        hitCount += hitTerrain ? 1 : 0;
        gridCount += raycastStats.usedGrid ? 1 : 0;
        ++frameCounter;

        if (frameCounter >= 120) {
            const double frames = static_cast<double>(frameCounter);
            std::cout
                << "Physics snap: "
                << accumulatedMs / frames << " ms, candidates "
                << static_cast<double>(accumulatedCandidateTriangles) / frames
                << ", tested "
                << static_cast<double>(accumulatedTestedTriangles) / frames
                << ", hits " << hitCount << "/" << frameCounter
                << ", grid " << gridCount << "/" << frameCounter
                << "\n";

            frameCounter = 0;
            accumulatedMs = 0.0;
            accumulatedTestedTriangles = 0;
            accumulatedCandidateTriangles = 0;
            hitCount = 0;
            gridCount = 0;
        }

        if (!hitTerrain)
            return false;

        if (transform->transform.position.y < hit.position.y + heightOffset)
            transform->transform.position.y = hit.position.y + heightOffset;
        return true;
    }
}
