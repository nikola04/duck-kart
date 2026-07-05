#include "Math.hpp"

#include <algorithm>
#include <cmath>

namespace engine::math {
    Uint32 mipLevels(Uint32 width, Uint32 height) {
        return 1u + static_cast<Uint32>(std::floor(std::log2(std::max(width,height))));
    }
}
