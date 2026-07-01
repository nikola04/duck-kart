#include "SDLContext.hpp"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>

namespace engine {
    SDLContext::SDLContext() {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    SDLContext::~SDLContext() {
        SDL_Quit();
    }
}
