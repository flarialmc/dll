#pragma once

#include "TexturePtr.hpp"

namespace mce {
    class TextureGroupBase {
    public:
        virtual void Destructor();
        virtual TexturePtr getTexture(const ResourceLocation& location, bool forceReload);
    };
}
