#include "LoadedModelBatcher.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace engine {
    namespace {
        struct MeshBatch {
            std::vector<Vertex> vertices;
            std::vector<std::uint32_t> indices;
            AABB bounds;
        };
    }

    LoadedModel LoadedModelBatcher::batchByMaterialAndChunk(LoadedModel model, float chunkSize) {
        std::unordered_map<ChunkBatchKey, MeshBatch, ChunkBatchKeyHash> batches;

        for (const LoadedMesh& loadedMesh : model.meshes) {
            const auto& vertices = loadedMesh.mesh.vertices();
            const auto& indices = loadedMesh.mesh.indices();

            for (std::size_t i = 0; i < indices.size(); i += 3) {
                const Vertex& a = vertices[indices[i + 0]];
                const Vertex& b = vertices[indices[i + 1]];
                const Vertex& c = vertices[indices[i + 2]];

                glm::vec3 centroid = (a.position + b.position + c.position) / 3.0f;

                ChunkBatchKey key = {
                    .x = static_cast<int>(std::floor(centroid.x / chunkSize)),
                    .z = static_cast<int>(std::floor(centroid.z / chunkSize)),
                    .material = loadedMesh.material,
                };

                MeshBatch& batch = batches[key];

                const std::uint32_t base = static_cast<std::uint32_t>(batch.vertices.size());

                batch.vertices.push_back(a);
                batch.vertices.push_back(b);
                batch.vertices.push_back(c);
                batch.indices.push_back(base);
                batch.indices.push_back(base + 1);
                batch.indices.push_back(base + 2);

                batch.bounds.expand(a.position);
                batch.bounds.expand(b.position);
                batch.bounds.expand(c.position);
            }
        }

        LoadedModel batchedModel;
        batchedModel.materials = std::move(model.materials);
        batchedModel.textures = std::move(model.textures);

        const std::size_t sourceMeshCount = model.meshes.size();

        for (auto& [key, batch] : batches) {
            if (batch.vertices.empty() || batch.indices.empty())
                continue;

            batchedModel.meshes.push_back(LoadedMesh{
                .mesh = Mesh{std::move(batch.vertices), std::move(batch.indices)},
                .material = key.material,
                .bounds = batch.bounds,
            });
        }

        std::cout << "Batched model by material and chunk: meshes " << sourceMeshCount
                  << " -> " << batchedModel.meshes.size()
                  << ", materials " << batchedModel.materials.size()
                  << ", textures " << batchedModel.textures.size()
                  << ", chunk size " << chunkSize
                  << std::endl;

        return batchedModel;
    }
}
