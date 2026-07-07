#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace engine {
    class Font {
        public:
            Font(const std::string& path, int size);
            ~Font();

            TTF_Font* handle() const {
                return m_font;
            }

        private:
            TTF_Font* m_font{};
    };
}
