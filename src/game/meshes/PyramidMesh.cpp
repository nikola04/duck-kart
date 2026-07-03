#include "PyramidMesh.hpp"

#include <cmath>

namespace game
{
    engine::Mesh createPyramidMesh()
    {
        std::vector<engine::Vertex> vertices = {
            // Position                    Normal

            // Base
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } }, // 0
            { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }, // 1
            { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }, // 2
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f } }, // 3

            // Top
            { {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 1.0f } }, // 4
        };

        std::vector<std::uint32_t> indices = {
            // Base
            0, 1, 2,
            0, 2, 3,

            // Sides
            0, 4, 1,
            1, 4, 2,
            2, 4, 3,
            3, 4, 0
        };

        constexpr float angle = 0.45f;
        const float c = std::cos(angle);
        const float s = std::sin(angle);

        for (auto& vertex : vertices)
        {
            const float x = vertex.position.x;
            const float z = vertex.position.z;

            vertex.position.x = x * c - z * s;
            vertex.position.z = x * s + z * c;
        }

        return engine::Mesh{
            std::move(vertices),
            std::move(indices)
        };
    }
}
