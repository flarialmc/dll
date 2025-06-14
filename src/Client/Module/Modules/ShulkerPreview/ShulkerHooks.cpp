#include "ShulkerHooks.hpp"
#include "BetterInventory.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Utils/Utils.hpp"
#include "Utils/Logger/Logger.hpp"
#include <format>

// ItemAppendFormattedHoverTextHook Implementation
ItemAppendFormattedHoverTextHook::ItemAppendFormattedHoverTextHook() : Hook("ItemAppendFormattedHoverText", 0) {}

void ItemAppendFormattedHoverTextHook::callback(void* self, ItemStack* itemStack, void* level, std::string* text, uint8_t a5) {
    // Call original function first
    ((original)funcOriginal)(self, itemStack, level, text, a5);
    
    Logger::custom(fg(fmt::color::yellow), "BetterInventory", "Item hover callback triggered!");
    
    if (!moduleInstance || !itemStack || !text) {
        Logger::custom(fg(fmt::color::red), "BetterInventory", std::format("Callback params check failed - moduleInstance: {}, itemStack: {}, text: {}", 
            (void*)moduleInstance, (void*)itemStack, (void*)text));
        return;
    }
    
    // Add durability information
    Item* item = itemStack->getItem();
    if (item) {
        short maxDamage = itemStack->getMaxDamage();
        if (maxDamage > 0) {
            short currentDamage = itemStack->getDamageValue();
            short durability = maxDamage - currentDamage;
            text->append(std::format("\n§7Durability: {} / {}", durability, maxDamage));
        }
        
        // Get item information from SDK
        std::string itemName = item->getname();
        std::string itemNamespace = item->getNamespace();
        
        // Check if this is a shulker box using our comprehensive detection
        bool isShulkerBox = BetterInventory::isShulkerBox(itemName);
        
        if (isShulkerBox) {
            // Handle shulker box specially
            text->append(std::format("\n§8{}:{}", itemNamespace, itemName));
            
            // Add shulker-specific functionality
            moduleInstance->handleShulkerHover(itemStack, text);
            return;
        }
        
        // Check for bee nest/beehive
        if (itemName == "bee_nest" || itemName == "beehive") {
            moduleInstance->AppendBeeNestInformation(*text, itemStack);
        }
        
        // Add item namespace and name for regular items
        text->append(std::format("\n§8{}:{}", itemNamespace, itemName));
    }
}

void ItemAppendFormattedHoverTextHook::enableHook() {
    // Hook the Item::appendFormattedHovertext function to add durability and ID information
    Logger::custom(fg(fmt::color::cyan), "BetterInventory", "Looking for Item::appendFormattedHovertext signature...");
    static auto addr = GET_SIG_ADDRESS("Item::appendFormattedHovertext");
    Logger::custom(fg(fmt::color::cyan), "BetterInventory", std::format("Signature address: 0x{:X}", (uintptr_t)addr));
    
    if (addr && addr != 0) {
        Logger::custom(fg(fmt::color::cyan), "BetterInventory", "Attempting to hook Item::appendFormattedHovertext...");
        this->manualHook((void*)addr, (void*)callback, (void**)&funcOriginal);
        Logger::custom(fg(fmt::color::lime_green), "BetterInventory", "Successfully hooked Item::appendFormattedHovertext");
    } else {
        Logger::custom(fg(fmt::color::red), "BetterInventory", "Item::appendFormattedHovertext signature not found - module will have limited functionality");
    }
}

// ShulkerAppendFormattedHoverTextHook Implementation
ShulkerAppendFormattedHoverTextHook::ShulkerAppendFormattedHoverTextHook() : Hook("ShulkerAppendFormattedHoverText", 0) {}

void ShulkerAppendFormattedHoverTextHook::callback(void* self, ItemStack* itemStack, void* level, std::string* text, uint8_t someBool) {
    if (!moduleInstance) return;
    
    // Call original function first
    ((original)funcOriginal)(self, itemStack, level, text, someBool);
    
    if (!itemStack || !text) return;
    
    // Get and increment the index
    int index = moduleInstance->getAndIncrementIndex();
    
    // Hide a secret index in the shulker name for identification
    text->append(std::format("§{:x}", index));
    
    // Reset the inventory for this index
    moduleInstance->resetShulkerInventory(index);
    
    // Load shulker box contents from NBT data
    CompoundTag* userData = static_cast<CompoundTag*>(itemStack->userData);
    if (userData && userData->contains("Items")) {
        const ListTag* items = userData->getList("Items");
        if (items) {
            moduleInstance->loadShulkerItems(index, items);
        }
    }
}

void ShulkerAppendFormattedHoverTextHook::enableHook() {
    // This would need proper signature scanning for ShulkerBoxBlockItem::appendFormattedHovertext
    static auto addr = GET_SIG_ADDRESS("ShulkerBoxBlockItem::appendFormattedHovertext");
    if (addr && addr != 0) {
        this->manualHook((void*)addr, (void*)callback, (void**)&funcOriginal);
    } else {
        Logger::custom(fg(fmt::color::orange), "BetterInventory", "ShulkerBoxBlockItem::appendFormattedHovertext signature not found - shulker preview will not work");
    }
}

// HoverRendererRenderHoverBoxHook Implementation
HoverRendererRenderHoverBoxHook::HoverRendererRenderHoverBoxHook() : Hook("HoverRendererRenderHoverBox", 0) {}

void HoverRendererRenderHoverBoxHook::callback(void* self, void* ctx, void* client, void* aabb, float someFloat) {
    if (!moduleInstance) {
        ((original)funcOriginal)(self, ctx, client, aabb, someFloat);
        return;
    }
    
    // Check if this is a shulker box hover by looking for the identifier
    std::string* filteredContent = moduleInstance->getHoverRendererContent(self);
    if (filteredContent && filteredContent->find("shulker_box") != std::string::npos) {
        // Extract the cached index from the hover text
        if (filteredContent->length() >= 7) {
            std::string cachedIndex = filteredContent->substr(filteredContent->length() - 1, 1);
            try {
                int index = std::stoi(cachedIndex, nullptr, 16);
                // Render the shulker preview instead of normal hover box
                moduleInstance->renderShulkerPreview(ctx, self, index);
                return;
            } catch (...) {
                // Fall back to normal rendering if index parsing fails
            }
        }
    }
    
    // Call original function for non-shulker items
    ((original)funcOriginal)(self, ctx, client, aabb, someFloat);
}

void HoverRendererRenderHoverBoxHook::enableHook() {
    // This would need proper signature scanning for HoverRenderer::_renderHoverBox
    Logger::custom(fg(fmt::color::cyan), "BetterInventory", "Looking for HoverRenderer::_renderHoverBox signature...");
    static auto addr = GET_SIG_ADDRESS("HoverRenderer::_renderHoverBox");
    Logger::custom(fg(fmt::color::cyan), "BetterInventory", std::format("HoverRenderer signature address: 0x{:X}", (uintptr_t)addr));
    
    if (addr && addr != 0) {
        Logger::custom(fg(fmt::color::cyan), "BetterInventory", "Attempting to hook HoverRenderer::_renderHoverBox...");
        this->manualHook((void*)addr, (void*)callback, (void**)&funcOriginal);
        Logger::custom(fg(fmt::color::lime_green), "BetterInventory", "Successfully hooked HoverRenderer::_renderHoverBox");
    } else {
        Logger::custom(fg(fmt::color::orange), "BetterInventory", "HoverRenderer::_renderHoverBox signature not found - hover box replacement will not work");
    }
}