#include "ItemRenderer.hpp"


void
ItemRenderer::renderGuiItemNew(BaseActorRenderContext *BaseActorRenderCtx, ItemStack *item, int mode, float x, float y,
                               float opacity, float scale, bool isEnchanted) {
    using func = void (__fastcall *)(ItemRenderer *, BaseActorRenderContext *, ItemStack *, int, float, float, bool,
                                     float, float, float);
    static func efunc = reinterpret_cast<func>(Memory::findSig(GET_SIG("ItemRenderer::renderGuiItemNew")));
    efunc(this, BaseActorRenderCtx, item, mode, x, y, isEnchanted, 1, opacity, scale);
}