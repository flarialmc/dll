#pragma once

#include <SDK/Client/Item/ItemStack.hpp>

class ItemRenderer {
public:
    void renderGuiItemNew(class BaseActorRenderContext *BaseActorRenderCtx, ItemStack *item, int mode, float x, float y,
                          float opacity, float scale, bool isEnchanted);
};