#pragma once

#include "../graphics/Material.hpp"
#include "../graphics/Mesh.hpp"
#include "../math/AABB.hpp"
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>

namespace engine {
    struct LoadedMaterial {
        glm::vec3 baseColor;
        int baseColorTexture = -1;
        int normalTexture = -1;

        float metallic = 0.0f;
        float roughness = 1.0f;

        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.5f;
    };

    struct LoadedTexture {
        std::vector<std::uint8_t> pixels;

        std::uint32_t width = 0;
        std::uint32_t height = 0;
    };

    struct LoadedGeometry {
         std::vector<Vertex> vertices;
         AABB bounds;
     };

    struct LoadedMesh {
        Mesh mesh;
        int material = -1;
        AABB bounds;
    };

    struct LoadedModel {
        std::vector<LoadedMesh> meshes;
        std::vector<LoadedMaterial> materials;
        std::vector<LoadedTexture> textures;
    };
}
