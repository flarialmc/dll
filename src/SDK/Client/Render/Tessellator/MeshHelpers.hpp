#pragma once

#include "Tessellator.hpp"
#include <SDK/Client/Render/BedrockTexture.hpp>
#include <SDK/Client/Render/ScreenContext.hpp>
#include <SDK/Client/Render/Material/MaterialPtr.hpp>

class MeshHelpers {
public:
    static void renderMeshImmediately(ScreenContext* screenContext, Tessellator* tessellator, mce::MaterialPtr* material);
    static void renderMeshImmediately2(ScreenContext* screenContext, Tessellator* tessellator, mce::MaterialPtr* material, BedrockTextureData& texture);
};
