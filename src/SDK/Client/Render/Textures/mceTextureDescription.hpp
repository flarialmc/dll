#pragma once

#include "TextureDescription.hpp"
#include "SampleDescription.hpp"
#include "Color.hpp"
#include "BindFlagsBit.hpp"

namespace mce {
    class TextureDescription : public cg::TextureDescription {
        mce::SampleDescription sampleDescription;
        mce::Color clearColor;
        float optimizedClearDepth;
        unsigned char optimizedClearStencil;
        mce::BindFlagsBit bindFlags;
        bool isStaging;
    };
}