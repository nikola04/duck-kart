#include "RendererGeometry.hpp"
#include "../Vertex.hpp"
#include <cstdint>
#include <vector>

namespace engine {
    Mesh createSkyboxMesh() {
        return Mesh{
            std::vector<Vertex>{
                {{-1, -1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1, -1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1,  1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{-1,  1, -1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{-1, -1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1, -1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{ 1,  1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{-1,  1,  1}, {0,0,1}, {0,0}, {1,0,0,1}},
            },
            std::vector<std::uint32_t>{
                0, 1, 2, 2, 3, 0,
                5, 4, 7, 7, 6, 5,
                4, 0, 3, 3, 7, 4,
                1, 5, 6, 6, 2, 1,
                3, 2, 6, 6, 7, 3,
                4, 5, 1, 1, 0, 4
            }
        };
    }

    Mesh createQuad2D(glm::vec2 position, glm::vec2 size) {
        float x = position.x;
        float y = position.y;
        float w = size.x;
        float h = size.y;

        return Mesh{
            std::vector<Vertex>{
                {{x,     y,     0.0f}, {0,0,1}, {0,0}, {1,0,0,1}},
                {{x + w, y,     0.0f}, {0,0,1}, {1,0}, {1,0,0,1}},
                {{x + w, y + h, 0.0f}, {0,0,1}, {1,1}, {1,0,0,1}},
                {{x,     y + h, 0.0f}, {0,0,1}, {0,1}, {1,0,0,1}},
            },
            std::vector<std::uint32_t>{
                0, 1, 2,
                2, 3, 0
            }
        };
    }
}
