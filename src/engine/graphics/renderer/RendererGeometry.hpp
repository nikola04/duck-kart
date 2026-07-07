#pragma once

#include "../Mesh.hpp"
#include <glm/vec2.hpp>

namespace engine {
    Mesh createSkyboxMesh();
    Mesh createQuad2D(glm::vec2 position, glm::vec2 size);
}
