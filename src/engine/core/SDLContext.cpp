#include "SDLContext.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include <string>

namespace engine {
    SDLContext::SDLContext() {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

        if (!TTF_Init())
            throw std::runtime_error(std::string("TTF_Init failed") + SDL_GetError());
    }

    SDLContext::~SDLContext() {
        TTF_Quit();
        SDL_Quit();
    }
}
