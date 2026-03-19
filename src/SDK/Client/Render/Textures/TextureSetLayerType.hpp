#pragma once

#include <cstdint>

namespace cg {
    // Values updated to match LeviLamina headers for 1.21.13x+.
    // Mers (3) and Subsurface (7) were inserted, shifting subsequent values.
    enum TextureSetLayerType : int32_t {
        Color      = 0,
        ColorUnlit = 1,
        MER        = 2,
        Mers       = 3,
        Metalness  = 4,
        Emissive   = 5,
        Roughness  = 6,
        Subsurface = 7,
        Normal     = 8,
        Heightmap  = 9,
        Count      = 10,
    };
}