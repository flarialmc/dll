#include "ItemRenderer.hpp"

#include <Utils/Utils.hpp>

void ItemRenderer::renderGuiItemNew(BaseActorRenderContext *BaseActorRenderCtx, ItemStack *item, int mode, float x, float y,
                               float opacity, float scale, bool isEnchanted) {
    using func = void (__fastcall *)(ItemRenderer *, BaseActorRenderContext *, ItemStack *, int, float, float, bool,
                                     float, float, float);
    // void renderGuiItemNew(class BaseActorRenderContext* baseActorRenderContext, class ItemStack* itemStack, int mode, float x, float y, float opacity, float scale, float a9, bool ench);
    using func2 = void (__fastcall *)(ItemRenderer *, BaseActorRenderContext *, ItemStack *, int, float, float, bool, float, float, float, int);
    static func efunc = reinterpret_cast<func>(GET_SIG_ADDRESS("ItemRenderer::renderGuiItemNew"));
    static func2 efunc2 = reinterpret_cast<func2>(GET_SIG_ADDRESS("ItemRenderer::renderGuiItemNew"));
    if (VersionUtils::checkBelow(21, 130)) return efunc(this, BaseActorRenderCtx, item, mode, x, y, isEnchanted, 1, opacity, scale);
    return efunc2(this, BaseActorRenderCtx, item, mode, x, y, isEnchanted, opacity, 1, scale, 17);
}

void* ItemRenderer::getAtlasIconInfo(ItemStack* item) {
    using func = void* (__fastcall *)(ItemRenderer *, ItemStack *);
    static func efunc = reinterpret_cast<func>(GET_SIG_ADDRESS("ItemRenderer::getAtlasIconInfo"));
    return efunc(this, item);
}

void ItemRenderer::iconBlit(BaseActorRenderContext *renderContext, void *textureObj,
                            float x, float y, float z, float *uvCoords,
                            float w, float h, float lightMultiplier, float alphaMultiplier,
                            int colorMult, int secondaryColorMult,
                            float xscale, float yScale, int iconBlitGlint,
                            bool useMultiColorTextureTinting) {
    using func = void (__fastcall *)(ItemRenderer *, BaseActorRenderContext *, void *,
                                     float, float, float, float *, float, float, float, float,
                                     int, int, float, float, int, bool);
    static func efunc = reinterpret_cast<func>(GET_SIG_ADDRESS("ItemRenderer::iconBlit"));
    efunc(this, renderContext, textureObj, x, y, z, uvCoords, w, h,
          lightMultiplier, alphaMultiplier, colorMult, secondaryColorMult,
          xscale, yScale, iconBlitGlint, useMultiColorTextureTinting);
}
