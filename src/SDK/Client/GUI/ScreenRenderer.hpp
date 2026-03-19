#pragma once

#include "../Render/ScreenContext.hpp"
#include "../Render/Material/MaterialPtr.hpp"
#include "IntRectangle.hpp"
#include "../Render/TexturePtr.hpp"

class ScreenRenderer {
public:
    static void blit(ScreenContext *screenContext, const TexturePtr* texture, const IntRectangle *rect, const mce::MaterialPtr *optionalMat);

    // Fills a rectangle with a solid color using Tessellator
    // x0,y0 = top-left corner, x1,y1 = bottom-right corner (typically screen dimensions)
    // This uses Tessellator which records commands on main thread (safe to call from hooks)
    static void fill(ScreenContext* screenContext, float x0, float y0, float x1, float y1, const mce::Color& color);
};
