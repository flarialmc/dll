#pragma once

#include <cstdint>
#include "TextureFormat.hpp"

namespace cg {
    struct ImageDescription {
        uint32_t width;
        uint32_t height;
        mce::TextureFormat textureFormat;
        uint32_t arraySize;
    };
}