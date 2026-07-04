#pragma once

#include <SDL3/SDL.h>
#include <filesystem>

namespace engine {
    enum class ShaderStage {
        Vertex,
        Fragment
    };

    class Shader {
        public:
            Shader(SDL_GPUDevice* device, const std::filesystem::path& path, ShaderStage stage, int samplers = 0);
            ~Shader();

            Shader(const Shader& shader) = delete;
            Shader& operator =(const Shader&) = delete;

            SDL_GPUShader* handle() const;

        private:
            SDL_GPUDevice* m_device;
            SDL_GPUShader* m_shader = nullptr;
    };
}
