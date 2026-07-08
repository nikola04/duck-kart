#pragma once

#include "LoadedModel.hpp"
#include <cstddef>
#include <functional>

namespace engine {
    struct ChunkBatchKey {
        int x = 0;
        int z = 0;
        int material = 0;

        bool operator ==(const ChunkBatchKey& other) const {
            return x == other.x && z == other.z && material == other.material;
        }
    };

    struct ChunkBatchKeyHash {
        std::size_t operator()(const ChunkBatchKey& key) const {
            std::size_t h = std::hash<int>{}(key.material);

            h ^= std::hash<int>{}(key.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(key.z) + 0x9e3779b9 + (h << 6) + (h >> 2);

            return h;
        }
    };

    class LoadedModelBatcher {
        public:
            static LoadedModel batchByMaterialAndChunk(LoadedModel model, float chunkSize);
    };
}
