#pragma once

namespace engine {
    struct WorldSettings {
        float chunkSize = 128.0f;
        int viewDistanceChunks = 8;
        int shadowViewDistancePaddingChunks = 1;
    };

    struct GraphicsSettings {
        bool shadowsEnabled = true;
        bool vsyncEnabled = true;
        bool generateMipmaps = true;
        bool anisotropyEnabled = true;
        float maxAnisotropy = 16.0f;
    };

    struct EngineSettings {
        WorldSettings world;
        GraphicsSettings graphics;
    };

    inline EngineSettings& settings() {
        static EngineSettings instance;
        return instance;
    }
}
