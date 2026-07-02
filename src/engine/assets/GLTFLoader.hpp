#pragma once

#include <filesystem>
#include "../graphics/Mesh.hpp"

namespace engine {
    class GLTFLoader {
        public:
            static Mesh loadMesh(const std::filesystem::path& path);
    };
}
