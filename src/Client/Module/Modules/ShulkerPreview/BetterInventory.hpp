#pragma once

#include "../Module.hpp"
#include "ShulkerRenderer.hpp"
#include "NBTStructures.hpp"
#include "../../../../SDK/Client/Core/ClientInstance.hpp"
#include "../../../../SDK/Client/Item/ItemStack.hpp"
#include "../../../../SDK/Client/Container/Inventory.hpp"
#include "Client/Events/EventManager.hpp"
#include "Client/GUI/Engine/Engine.hpp"
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <chrono>

#define SHULKER_CACHE_SIZE 16

class BetterInventory : public Module {
private:
    ShulkerRenderer shulkerRenderer;
    ItemStack shulkerInventory[SHULKER_CACHE_SIZE][27];
    std::string shulkerItemNames[SHULKER_CACHE_SIZE][27];
    int index = 0;
    
    // Last hovered item tracking
    ItemStack* lastHoveredItem = nullptr;
    std::string lastHoveredItemName = "";
    std::chrono::steady_clock::time_point lastHoverTime;
    
    // Shulker preview rendering state
    bool shouldRenderShulkerPreview = false;
    int currentShulkerIndex = -1;
    std::string shulkerPreviewItemName = "";
    Vec2<float> lastCursorPos = Vec2<float>(0, 0);
    std::string lastHoveredShulkerKey = "";
    
public:
    BetterInventory() : Module("ShulkerPreview", "Enhanced inventory features\nincluding shulker box preview.",
        IDR_INVENTORY_PNG, "") {
    };

    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;
    
    // Event handlers
    void onContainerSlotHovered(ContainerSlotHoveredEvent& event);
    void onSetupAndRender(SetupAndRenderEvent& event);
    void onRender(RenderEvent& event);
    
    // Public interface for hooks
    int getAndIncrementIndex();
    void resetShulkerInventory(int index);
    void loadShulkerItems(int index, const ListTag* items);
    void renderShulkerPreview(void* ctx, void* hoverRenderer, int index);
    std::string* getHoverRendererContent(void* hoverRenderer);
    
    // Utility functions
    void AppendBeeNestInformation(std::string& text, ItemStack* itemStack);
    void handleShulkerHover(ItemStack* itemStack, std::string* text);
    static bool isShulkerBox(const std::string& itemName);
    
    // SDK-based tooltip methods
    void displayShulkerTooltip(ItemStack* itemStack, const std::string& itemName);
    void displayItemInfo(ItemStack* itemStack, const std::string& itemName, const std::string& itemNamespace);
    
    // Getters for external access
    ShulkerRenderer& getShulkerRenderer() { return shulkerRenderer; }
    ItemStack* getShulkerInventory() { return &shulkerInventory[0][0]; }
};