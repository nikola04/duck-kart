#pragma once

#include "LoadedModel.hpp"
#include <filesystem>

namespace engine {
    class ModelLoader {
        public:
            virtual ~ModelLoader() = default;

            virtual LoadedModel loadModel(const std::filesystem::path& path) const = 0;
    };
}
