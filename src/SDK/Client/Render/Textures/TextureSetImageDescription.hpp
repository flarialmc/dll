#pragma once

#include <variant>
#include "TextureSetLayerType.hpp"
#include "ImageDescription.hpp"
#include "ColorChannel.hpp"
#include "Color.hpp"

namespace cg {
    class TextureSetImageDescription {
    public:
        class LayerInfoVar {
            cg::TextureSetLayerType layerType;
            std::variant <cg::ImageDescription, ColorChannel, mce::Color> data;
        };

        std::vector <cg::TextureSetImageDescription::LayerInfoVar, std::allocator<cg::TextureSetImageDescription::LayerInfoVar>> layerInfo;
    };
}