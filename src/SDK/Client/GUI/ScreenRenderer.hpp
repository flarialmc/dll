#pragma once

#include "../Render/ScreenContext.hpp"
#include "../Render/Material/MaterialPtr.hpp"
#include "IntRectangle.hpp"
#include "../Render/TexturePtr.hpp"

class ScreenRenderer {
public:
    static void blit(ScreenContext *screenContext, const TexturePtr* texture, const IntRectangle *rect, const mce::MaterialPtr *optionalMat);
};
