#pragma once

#include "Camera.hpp"
#include "RenderChunk.hpp"
#include "RenderObject.hpp"
#include "Skybox.hpp"
#include "../settings/EngineSettings.hpp"
#include "../world/World.hpp"
#include <cmath>
#include <cstdint>
#include <glm/vec3.hpp>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine {
    struct RenderModel {
        std::vector<RenderObject> objects;
    };

    struct DynamicRenderModel {
        Entity entity = NullEntity;
        RenderModel model;
        Transform localTransform;
    };

    struct SceneStats {
        std::size_t chunks = 0;
        std::size_t objects = 0;
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

    struct FogSettings {
        bool enabled = true;
        glm::vec3 color{0.62f, 0.72f, 0.82f};
        float startFactor = 0.55f;
        float endFactor = 0.95f;
    };

    struct Scene {
        Camera camera;
        Skybox skybox;
        DirectionalLight sun;
        FogSettings fog;

        std::unordered_map<ChunkCoords, RenderChunk, ChunkCoordsHash> chunks;
        std::vector<DynamicRenderModel> dynamicModels;
        std::vector<PointLight> pointLights;
        WorldSettings worldSettings = settings().world;

        void clear() {
            chunks.clear();
            dynamicModels.clear();
            uiTextures.clear();
        }

        void addModel(RenderModel&& model) {
            const std::size_t addedObjects = model.objects.size();

            for (auto& object : model.objects) {
                ChunkCoords coords = chunkCoordsFromBounds(object.bounds);
                RenderChunk& chunk = chunks[coords];
                chunk.bounds.expand(object.bounds.min);
                chunk.bounds.expand(object.bounds.max);
                chunk.objects.push_back(std::move(object));
            }

            const SceneStats currentStats = stats();
            std::cout << "Scene addModel: added objects " << addedObjects
                      << ", total chunks " << currentStats.chunks
                      << ", total objects " << currentStats.objects
                      << std::endl;
        }

        void addDynamicModel(Entity entity, RenderModel&& model, const Transform& localTransform = {}) {
            const std::size_t addedObjects = model.objects.size();
            dynamicModels.push_back(DynamicRenderModel{
                .entity = entity,
                .model = std::move(model),
                .localTransform = localTransform,
            });

            std::cout << "Scene addDynamicModel: entity " << entity
                      << ", added objects " << addedObjects
                      << ", total dynamic models " << dynamicModels.size()
                      << std::endl;
        }

        void updateDynamicModelTransform(Entity entity, const Transform& transform) {
            for (auto& dynamicModel : dynamicModels) {
                if (dynamicModel.entity != entity)
                    continue;

                for (auto& object : dynamicModel.model.objects) {
                    Transform objectTransform = transform;
                    objectTransform.position += dynamicModel.localTransform.position;
                    objectTransform.rotation += dynamicModel.localTransform.rotation;
                    objectTransform.scale *= dynamicModel.localTransform.scale;

                    object.transform = objectTransform;
                    object.bounds = object.localBounds.transformed(objectTransform.matrix());
                }

                return;
            }
        }

        const DynamicRenderModel* dynamicModel(Entity entity) const {
            for (const auto& dynamicModel : dynamicModels) {
                if (dynamicModel.entity == entity)
                    return &dynamicModel;
            }

            return nullptr;
        }

        SceneStats stats() const {
            SceneStats result{};
            result.chunks = chunks.size();

            for (const auto& [coords, chunk] : chunks)
                result.objects += chunk.objects.size();

            for (const auto& dynamicModel : dynamicModels)
                result.objects += dynamicModel.model.objects.size();

            return result;
        }

        ChunkCoords chunkCoordsFromPosition(const glm::vec3& position) const {
                  return {
                  .x = static_cast<std::int32_t>(std::floor(position.x / worldSettings.chunkSize)),
                  .z = static_cast<std::int32_t>(std::floor(position.z / worldSettings.chunkSize))
              };
          }

        std::vector<UITexture> uiTextures;

        private:
            ChunkCoords chunkCoordsFromBounds(const AABB& bounds) const {
                const glm::vec3 center = (bounds.min + bounds.max) * 0.5f;

                return ChunkCoords{
                    .x = static_cast<std::int32_t>(std::floor(center.x / worldSettings.chunkSize)),
                    .z = static_cast<std::int32_t>(std::floor(center.z / worldSettings.chunkSize))
                };
            }
    };
}
