#include "TriangleMesh.hpp"

namespace game
{
    engine::Mesh createTriangleMesh()
    {
        std::vector<engine::Vertex> vertices = {
            // x      y      z      r     g     b
            {  0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f },
            {  0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f },
            { -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f },
        };

        std::vector<std::uint32_t> indices = {
            0, 1, 2
        };

        return engine::Mesh{
            std::move(vertices),
            std::move(indices)
        };
    }
}
