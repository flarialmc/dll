#pragma once


#include "../Core/ClientInstance.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "ScreenContext.hpp"
#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../Level/Level.hpp"
#include "../Container/Inventory.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Client/GUI/D2D.hpp"
#include "TextMeasureData.hpp"
#include "Texture2D.hpp"

class MinecraftUIRenderContext {
public:
    BUILD_ACCESS(this, ClientInstance*, clientInstance, GET_OFFSET("MinecraftUIRenderContext::clientInstance"));
    BUILD_ACCESS(this, ScreenContext*, screenContext, GET_OFFSET("MinecraftUIRenderContext::screenContext"));
    BUILD_ACCESS(this, float, FrameAlpha, 0x0);

    static MaterialPtr *getUIMaterial() {
        static uintptr_t ref;

        if (ref == 0) {

            //The MaterialPtr is called "ui_fill_color, it has a static pointer to its instance.

            //ScreenRenderer::fillRectangle
            ref = Memory::findSig(GET_SIG("MinecraftUIRenderContext::getUIMaterial"));
        }

        int re = *reinterpret_cast<int *>(ref + 3);
        return reinterpret_cast<MaterialPtr *>(ref + re + 7);
    }
};