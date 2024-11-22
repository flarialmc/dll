#pragma once

#include "Tessellator.hpp"
#include "../ScreenContext.hpp"
#include "../Material/MaterialPtr.hpp"

class MeshHelpers {
public:
    static void renderMeshImmediately(ScreenContext* screenContext, Tessellator* tessellator, mce::MaterialPtr* material);
};
