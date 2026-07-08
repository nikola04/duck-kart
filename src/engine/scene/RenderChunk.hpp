#pragma once

#include "RenderObject.hpp"
#include "../math/AABB.hpp"
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace engine {
    struct ChunkCoords {
        std::int32_t x, z;

        bool operator ==(const ChunkCoords& other) const {
            return other.x == x && other.z == z;
        }

        bool isInViewDistance(const ChunkCoords& other, int viewDistance) const {
            int dx = std::abs(x - other.x);
            int dz = std::abs(z - other.z);

            return dx * dx + dz * dz <= viewDistance * viewDistance;
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
