#pragma once

#include <SDK/Client/Item/ItemStack.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

#include <cstddef>
#include <cstdint>

// ArmorRenderInfo temporary buffer size used by BetterInventory.
// ArmorRenderInfo_loadFromItemStack writes fields up to +0xD0.
constexpr size_t ARMOR_RENDER_INFO_SIZE = 288;
// Leather ArmorSlot index in BlockActorRenderDispatcher's armor slot array.
constexpr uint8_t LEATHER_ARMOR_SLOT_INDEX = 16;

class ItemRenderer {
public:
    void renderGuiItemNew(class BaseActorRenderContext *BaseActorRenderCtx, ItemStack *item, int mode, float x, float y,
                          float opacity, float scale, bool isEnchanted);

    // Look up atlas texture handle for an item.
    // Returns pointer to atlas cache entry at (node+24): sprite UV shared_ptr.
    // For iconBlit, pass (return_value + 16) to reach the texture handle shared_ptr
    // at (node+40). This matches what renderGuiItemNew does internally.
    void* getAtlasIconInfo(ItemStack* item);

    // 2D icon quad renderer. Selects material by useMultiColorTextureTinting:
    //   false -> ItemRenderer+0x1C0 "ui_texture_and_color" (single tint)
    //   true  -> ItemRenderer+0x1D0 "ui_texture_and_color_tint" (primary + secondary overlay)
    void iconBlit(class BaseActorRenderContext *renderContext,
                  void *textureObj,
                  float x, float y, float z,
                  float *uvCoords,
                  float w, float h,
                  float lightMultiplier, float alphaMultiplier,
                  int colorMult, int secondaryColorMult,
                  float xscale, float yScale,
                  int iconBlitGlint,
                  bool useMultiColorTextureTinting);

    // Returns a pointer to ItemRenderer::armorRenderInfo.
    // When unavailable for a version, returns nullptr.
    void** getArmorRenderInfoSlot() {
        const int offset = GET_OFFSET("ItemRenderer::armorRenderInfo");
        if (offset <= 0) return nullptr;
        return reinterpret_cast<void**>(reinterpret_cast<char*>(this) + offset);
    }
};
