#pragma once

#include <cstdint>

namespace cg {
    enum TextureSetLayerType : int32_t {
        Color = 0x0000,
        ColorUnlit = 0x0001,
        MER = 0x0002,
        Metalness = 0x0003,
        Emissive = 0x0004,
        Roughness = 0x0005,
        Normal = 0x0006,
        Heightmap = 0x0007,
    };
}