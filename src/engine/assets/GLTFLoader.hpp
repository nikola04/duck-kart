#pragma once

#include <filesystem>
#include "../graphics/Mesh.hpp"

namespace engine {
    struct LoadedMaterial {
        glm::vec3 baseColor;
        int baseColorTexture = -1;
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
