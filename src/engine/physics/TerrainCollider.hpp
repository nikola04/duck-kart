#pragma once

#include "../assets/LoadedModel.hpp"
#include "../math/Transform.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

namespace engine {
    struct TerrainTriangle {
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;
        glm::vec3 normal;
    };

    struct RaycastHit {
        glm::vec3 position{};
        glm::vec3 normal{0.0f, 1.0f, 0.0f};
        float distance = 0.0f;
    };

    struct TerrainRaycastStats {
        std::size_t testedTriangles = 0;
        std::size_t candidateTriangles = 0;
        bool usedGrid = false;
        bool hit = false;
    };

    class TerrainCollider {
        public:
            explicit TerrainCollider(float cellSize = 128.0f);

            void addModel(const LoadedModel& model, const Transform& transform);

            bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit) const;
            bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const;

            std::size_t triangleCount() const { return m_triangles.size(); }
            std::size_t cellCount() const { return m_cells.size(); }

        private:
            struct CellCoords {
                std::int32_t x = 0;
                std::int32_t z = 0;

                bool operator==(const CellCoords& other) const {
                    return x == other.x && z == other.z;
                }
            };

            struct CellCoordsHash {
                std::size_t operator()(const CellCoords& coords) const {
                    const auto x = static_cast<std::uint32_t>(coords.x);
                    const auto z = static_cast<std::uint32_t>(coords.z);
                    return (static_cast<std::size_t>(x) << 32) ^ static_cast<std::size_t>(z);
                }
            };

            CellCoords cellCoordsFromPosition(const glm::vec3& position) const;
            void addTriangleToCells(std::size_t triangleIndex);
            bool raycastTriangles(const std::vector<std::size_t>& triangleIndices, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const;
            bool raycastAllTriangles(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const;

            float m_cellSize;
            std::vector<TerrainTriangle> m_triangles;
            std::unordered_map<CellCoords, std::vector<std::size_t>, CellCoordsHash> m_cells;
    };
}
