#pragma once

#include <SDL3/SDL.h>

namespace engine
{
    template <typename T>
    struct VertexLayout
    {
        static SDL_GPUVertexInputState create() = delete;
    };
}
