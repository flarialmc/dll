#include "ShulkerRenderer.hpp"
#include "../../../../SDK/Client/Core/ClientInstance.hpp"
#include "../../../../SDK/Client/Render/Textures/Color.hpp"
#include "../../../../SDK/Client/Render/ResourceLocation.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <format>

// Texture loading
static HashedString flushString("ui_flush");

int ShulkerRenderer::countNewlines(const std::string& str) {
    int newlineCount = 0;
    for (char c : str) {
        if (c == '\n') {
            newlineCount++;
        }
    }
    return newlineCount;
}

void ShulkerRenderer::Render(MinecraftUIRenderContext* ctx, const std::string& hoverText, Vec2<float> cursorPos, Vec2<float> offset, int index, ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27]) {
    if (!ctx) return;
    
    // Simplified implementation to avoid API conflicts
    // Just log that we would render for now
    
    Logger::custom(fg(fmt::color::cyan), "ShulkerRenderer", 
        std::format("Would render shulker preview at ({}, {}) for index {}", 
                   cursorPos.x + offset.x, cursorPos.y + offset.y, index));
    
    // Count items to display in log
    int itemCount = 0;
    if (index >= 0 && index < SHULKER_CACHE_SIZE) {
        for (int i = 0; i < 27; i++) {
            if (shulkerInventory[index][i].getItem() != nullptr) {
                itemCount++;
            }
        }
    }
    
    Logger::custom(fg(fmt::color::lime_green), "ShulkerRenderer", 
        std::format("Shulker contains {} items", itemCount));
    
    // TODO: Implement full rendering once API conflicts are resolved
}