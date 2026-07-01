#pragma once

#include "VertexLayout.hpp"
#include <cstddef>
namespace engine {
    struct Vertex {
        float x, y, z;
        float r, g, b;
    };

    template<>
    struct VertexLayout<Vertex> {
        static SDL_GPUVertexInputState create() {
            static SDL_GPUVertexBufferDescription buffer_desc{};
            static SDL_GPUVertexAttribute attributes[2]{};

            buffer_desc.slot = 0;
            buffer_desc.pitch = sizeof(Vertex);
            buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
            buffer_desc.instance_step_rate = 0;

            attributes[0].location = 0;
            attributes[0].buffer_slot = 0;
            attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
            attributes[0].offset = offsetof(Vertex, x);

            attributes[1].location = 1;
            attributes[1].buffer_slot = 0;
            attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
            attributes[1].offset = offsetof(Vertex, r);

            SDL_GPUVertexInputState state{};
            state.vertex_buffer_descriptions = &buffer_desc;
            state.num_vertex_buffers = 1;
            state.vertex_attributes = attributes;
            state.num_vertex_attributes = 2;

            return state;
        }
    };
}
