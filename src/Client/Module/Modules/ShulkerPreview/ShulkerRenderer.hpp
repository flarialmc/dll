#pragma once
#include "../../../../SDK/Client/Item/ItemStack.hpp"
#include "../../../../SDK/Client/Core/HashedString.hpp"
#include "../../../../SDK/Client/Render/TexturePtr.hpp"
#include "../../../../SDK/Client/Render/BaseActorRenderContext.hpp"
#include "../../../../SDK/Client/Render/MinecraftUIRenderContext.hpp"
#include "../../../../SDK/Client/Render/Textures/Color.hpp"
#include "../../../../SDK/Client/GUI/RectangleArea.hpp"
#include "../../../../Utils/Utils.hpp"
#include <string>

#define SHULKER_CACHE_SIZE 16

struct TextMeasureData {
    float fontSize;
    // Add other fields as needed
};

class NinesliceHelper {
public:
    int left, right, top, bottom;
    
    NinesliceHelper(int l, int r, int t, int b) : left(l), right(r), top(t), bottom(b) {}
    
    void Draw(float x1, float x2, float y1, float y2, TexturePtr* texture, MinecraftUIRenderContext* ctx) {
        // Simple implementation - draw as single texture for now
        Vec2<float> pos(x1, y1);
        Vec2<float> size(x2 - x1, y2 - y1);
        Vec2<float> uv(0.0f, 0.0f);
        Vec2<float> uvSize(1.0f, 1.0f);
        ctx->drawImage(*texture, pos, size, uv, uvSize);
    }
};

class ShulkerRenderer {
private:
    TexturePtr mBackgroundTexture;
    TexturePtr mItemSlotTexture;
    bool hasLoadedTexture = false;
    
    // Slot sizing
    float slotSize = 20.0f;
    float borderSize = 2.0f; // (slotSize - 16.0f) / 2
    
    // UV positions for GUI textures
    Vec2<float> itemSlotUvPos = Vec2<float>(188.0f / 256.0f, 184.0f / 256.0f);
    Vec2<float> itemSlotUvSize = Vec2<float>(22.0f / 256.0f, 22.0f / 256.0f);
    
    // Nineslice helper for background
    NinesliceHelper backgroundNineslice;
    
    int countNewlines(const std::string& str);

public:
    ShulkerRenderer() : backgroundNineslice(16, 16, 4, 4) {}
    
    void Render(MinecraftUIRenderContext* ctx, const std::string& hoverText, Vec2<float> cursorPos, Vec2<float> offset, int index, ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27]);
};