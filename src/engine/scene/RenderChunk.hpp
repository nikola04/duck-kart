#pragma once

#include "RenderObject.hpp"
#include "../math/AABB.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace engine {
    constexpr float DefaultChunkSize = 128.0f;

    struct ChunkCoords {
        std::int32_t x, z;

        bool operator ==(const ChunkCoords& other) const {
            return other.x == x && other.z == z;
        }
    };

    struct ChunkCoordsHash {
        std::size_t operator ()(const ChunkCoords& coords) const {
            std::size_t h = std::hash<int>{}(coords.x);
            return h ^ std::hash<int>{}(coords.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
    };

    struct RenderChunk {
        AABB bounds;
        std::vector<RenderObject> objects;
    };
}
