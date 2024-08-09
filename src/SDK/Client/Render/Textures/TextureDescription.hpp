#pragma once

#include <cstdint>
#include "ImageDescription.hpp"

namespace cg {
    class TextureDescription : public cg::ImageDescription {
        uint32_t mipCount;
    };
}