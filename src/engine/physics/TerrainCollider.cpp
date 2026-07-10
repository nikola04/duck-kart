#include "TerrainCollider.hpp"
#include <cmath>
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/geometric.hpp>

namespace engine {
    TerrainCollider::TerrainCollider(float cellSize)
        : m_cellSize(cellSize) {}

    void TerrainCollider::addModel(const LoadedModel& model, const Transform& transform) {
        glm::mat4 matrix = transform.matrix();

        for (const auto& mesh : model.meshes) {
            const auto& vertices = mesh.mesh.vertices();
            const auto& indices = mesh.mesh.indices();

            for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
                glm::vec3 a = glm::vec3(matrix * glm::vec4(vertices[indices[i + 0]].position, 1.0f));
                glm::vec3 b = glm::vec3(matrix * glm::vec4(vertices[indices[i + 1]].position, 1.0f));
                glm::vec3 c = glm::vec3(matrix * glm::vec4(vertices[indices[i + 2]].position, 1.0f));

                glm::vec3 normal = glm::cross(b - a, c - a);
                if (glm::dot(normal, normal) <= 0.000001f)
                    continue;

                normal = glm::normalize(normal);

                const std::size_t triangleIndex = m_triangles.size();
                m_triangles.push_back({
                    a,
                    b,
                    c,
                    normal
                });
                addTriangleToCells(triangleIndex);
            }
        }
    }

    TerrainCollider::CellCoords TerrainCollider::cellCoordsFromPosition(const glm::vec3& position) const {
        return {
            static_cast<std::int32_t>(std::floor(position.x / m_cellSize)),
            static_cast<std::int32_t>(std::floor(position.z / m_cellSize)),
        };
    }

    void TerrainCollider::addTriangleToCells(std::size_t triangleIndex) {
        const auto& triangle = m_triangles[triangleIndex];

        const float minX = std::min({ triangle.a.x, triangle.b.x, triangle.c.x });
        const float maxX = std::max({ triangle.a.x, triangle.b.x, triangle.c.x });
        const float minZ = std::min({ triangle.a.z, triangle.b.z, triangle.c.z });
        const float maxZ = std::max({ triangle.a.z, triangle.b.z, triangle.c.z });

        const auto minCell = cellCoordsFromPosition({ minX, 0.0f, minZ });
        const auto maxCell = cellCoordsFromPosition({ maxX, 0.0f, maxZ });

        for (std::int32_t x = minCell.x; x <= maxCell.x; ++x) {
            for (std::int32_t z = minCell.z; z <= maxCell.z; ++z) {
                m_cells[{ x, z }].push_back(triangleIndex);
            }
        }
    }

    static bool intersectRayTriangle(const glm::vec3& origin, const glm::vec3& direction, const TerrainTriangle& triangle, float& distance) {
        constexpr float epsilon = 0.000001f;

        glm::vec3 edge1 = triangle.b - triangle.a;
        glm::vec3 edge2 = triangle.c - triangle.a;

        glm::vec3 h = glm::cross(direction, edge2);
        float det = glm::dot(edge1, h);

        if (det > -epsilon && det < epsilon)
            return false;

        float invDet = 1.0f / det;

        glm::vec3 s = origin - triangle.a;
        float u = invDet * glm::dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return false;

        glm::vec3 q = glm::cross(s, edge1);
        float v = invDet * glm::dot(direction, q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        float t = invDet * glm::dot(edge2, q);

        if (t <= epsilon)
            return false;

        distance = t;
        return true;
    }

    bool TerrainCollider::raycastTriangles(const std::vector<std::size_t>& triangleIndices, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const {
        bool foundHit = false;
        float closestDistance = maxDistance;
        stats.candidateTriangles += triangleIndices.size();

        for (std::size_t triangleIndex : triangleIndices) {
            const auto& triangle = m_triangles[triangleIndex];
            float distance = 0.0f;
            ++stats.testedTriangles;

            if (!intersectRayTriangle(origin, direction, triangle, distance))
                continue;

            if (distance < 0.0f || distance > closestDistance)
                continue;

            closestDistance = distance;
            hit.position = origin + direction * distance;
            hit.normal = triangle.normal;
            hit.distance = distance;
            foundHit = true;
        }

        stats.hit = foundHit;
        return foundHit;
    }

    bool TerrainCollider::raycastAllTriangles(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const {
        bool foundHit = false;
        float closestDistance = maxDistance;
        stats.candidateTriangles += m_triangles.size();

        for (const auto& triangle : m_triangles) {
            float distance = 0.0f;
            ++stats.testedTriangles;

            if (!intersectRayTriangle(origin, direction, triangle, distance))
                continue;

            if (distance < 0.0f || distance > closestDistance)
                continue;

            closestDistance = distance;
            hit.position = origin + direction * distance;
            hit.normal = triangle.normal;
            hit.distance = distance;
            foundHit = true;
        }

        stats.hit = foundHit;
        return foundHit;
    }

    bool TerrainCollider::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit) const {
        TerrainRaycastStats stats;
        return raycast(origin, direction, maxDistance, hit, stats);
    }

    bool TerrainCollider::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit, TerrainRaycastStats& stats) const {
        stats = {};

        if (maxDistance <= 0.0f || glm::dot(direction, direction) <= 0.000001f)
            return false;

        const glm::vec3 rayDirection = glm::normalize(direction);

        if (std::abs(rayDirection.x) <= 0.0001f && std::abs(rayDirection.z) <= 0.0001f) {
            stats.usedGrid = true;
            const auto cell = cellCoordsFromPosition(origin);
            const auto it = m_cells.find(cell);
            if (it == m_cells.end())
                return false;

            return raycastTriangles(it->second, origin, rayDirection, maxDistance, hit, stats);
        }

        return raycastAllTriangles(origin, rayDirection, maxDistance, hit, stats);
    }
}
