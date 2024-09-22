#pragma once

#include <array>
#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/TextureGroup.hpp"
#include "../../../../SDK/Client/Render/ResourceLocation.hpp"
#include "../../../../SDK/Client/Render/TexturePtr.hpp"

class TextureGroup_getTextureHook : public Hook {
private:
    static TexturePtr *TextureGroup_getTextureCallback(TextureGroup* _this, TexturePtr* result, ResourceLocation* location, bool forceReload, void* a5, int a6, void* a7);

public:
    typedef TexturePtr *(__thiscall *original)(TextureGroup* _this, TexturePtr* result, ResourceLocation* location, bool forceReload, void* a5, int a6, void* a7);

    static inline original funcOriginal = nullptr;

    TextureGroup_getTextureHook();

    void enableHook() override;
};
