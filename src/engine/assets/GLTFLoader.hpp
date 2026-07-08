#pragma once

#include "ModelLoader.hpp"
#include <filesystem>

namespace engine {
    class GLTFLoader final : public ModelLoader {
        public:
            LoadedModel loadModel(const std::filesystem::path& path) const override;
    };
}
