#pragma once

#include <filesystem>
#include "../graphics/Mesh.hpp"
#include "../graphics/Material.hpp"

namespace engine {
    struct LoadedMaterial {
        glm::vec3 baseColor;
        int baseColorTexture = -1;

        float metallic = 0.0f;
        float roughness = 1.0f;

        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.5f;
    };

    struct LoadedTexture {
        std::vector<std::uint8_t> pixels;

        uint32_t width;
        uint32_t height;
    };

    struct LoadedMesh {
        Mesh mesh;

        int material = -1;
    };

    struct LoadedModel {
        std::vector<LoadedMesh> meshes;
        std::vector<LoadedMaterial> materials;
        std::vector<LoadedTexture> textures;
    };

    class GLTFLoader {
        public:
            static LoadedModel loadModel(const std::filesystem::path& path);
    };
}
