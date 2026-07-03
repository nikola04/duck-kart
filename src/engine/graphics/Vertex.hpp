#pragma once

#include "VertexLayout.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace engine {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    template<>
    struct VertexLayout<Vertex> {
        static SDL_GPUVertexInputState create() {
            static SDL_GPUVertexBufferDescription buffer_desc{};
            static SDL_GPUVertexAttribute attributes[3]{};

            buffer_desc.slot = 0;
            buffer_desc.pitch = sizeof(Vertex);
            buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
            buffer_desc.instance_step_rate = 0;

            attributes[0].location = 0;
            attributes[0].buffer_slot = 0;
            attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
            attributes[0].offset = offsetof(Vertex, position);

            attributes[1].location = 1;
            attributes[1].buffer_slot = 0;
            attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
            attributes[1].offset = offsetof(Vertex, normal);

            attributes[2].location = 2;
            attributes[2].buffer_slot = 0;
            attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
            attributes[2].offset = offsetof(Vertex, uv);

            SDL_GPUVertexInputState state{};
            state.vertex_buffer_descriptions = &buffer_desc;
            state.num_vertex_buffers = 1;
            state.vertex_attributes = attributes;
            state.num_vertex_attributes = 3;

            return state;
        }
    };
}
