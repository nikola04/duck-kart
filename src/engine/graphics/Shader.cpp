#include "Shader.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <vector>

namespace engine {
    static std::vector<char> readFile(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
            throw std::runtime_error("Failed to open file: " + path.string());

        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(static_cast<std::size_t>(size));

        if (!file.read(buffer.data(), size))
            throw std::runtime_error("Failed to read file: " + path.string());

        return buffer;
    }

    Shader::Shader(SDL_GPUDevice* device, const std::filesystem::path& path, ShaderStage stage, int samplers): m_device(device) {
        const std::vector<char> code = readFile(path);

        SDL_GPUShaderCreateInfo info{};
        info.code = reinterpret_cast<const Uint8*>(code.data());
        info.code_size = code.size();
        info.format = SDL_GPU_SHADERFORMAT_MSL;
        info.entrypoint = "main0";

        switch (stage) {
            case engine::ShaderStage::Vertex:
                info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
                info.num_uniform_buffers = 1;
                break;
            case engine::ShaderStage::Fragment:
                info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
                info.num_samplers = samplers;
                info.num_uniform_buffers = 4;
                break;
        }

        m_shader = SDL_CreateGPUShader(m_device, &info);

        if (!m_shader)
            throw std::runtime_error(std::string("SDL_CreateGPUShader failed: ") + SDL_GetError());
    }

    Shader::~Shader() {
        if (m_shader)
            SDL_ReleaseGPUShader(m_device, m_shader);
    }

    SDL_GPUShader* Shader::handle() const {
        return m_shader;
    }
}
