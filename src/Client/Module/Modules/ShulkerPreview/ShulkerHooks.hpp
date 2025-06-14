#pragma once

#include "../../../Hook/Hooks/Hook.hpp"
#include "NBTStructures.hpp"
#include "../../../../SDK/Client/Item/ItemStack.hpp"
#include "../../../../SDK/Client/Item/Item.hpp"
#include "../../../../SDK/Client/Core/ClientInstance.hpp"

class BetterInventory; // Forward declaration

class ItemAppendFormattedHoverTextHook : public Hook {
private:
    static void callback(void* self, ItemStack* itemStack, void* level, std::string* text, uint8_t a5);

public:
    typedef void(__thiscall *original)(void*, ItemStack*, void*, std::string*, uint8_t);
    static inline void* funcOriginal = nullptr;
    static inline BetterInventory* moduleInstance = nullptr;

    ItemAppendFormattedHoverTextHook();
    void enableHook() override;
    static void setModuleInstance(BetterInventory* instance) { moduleInstance = instance; }
};

class ShulkerAppendFormattedHoverTextHook : public Hook {
private:
    static void callback(void* self, ItemStack* itemStack, void* level, std::string* text, uint8_t someBool);

public:
    typedef void(__thiscall *original)(void*, ItemStack*, void*, std::string*, uint8_t);
    static inline void* funcOriginal = nullptr;
    static inline BetterInventory* moduleInstance = nullptr;

    ShulkerAppendFormattedHoverTextHook();
    void enableHook() override;
    static void setModuleInstance(BetterInventory* instance) { moduleInstance = instance; }
};

class HoverRendererRenderHoverBoxHook : public Hook {
private:
    static void callback(void* self, void* ctx, void* client, void* aabb, float someFloat);

public:
    typedef void(__thiscall *original)(void*, void*, void*, void*, float);
    static inline void* funcOriginal = nullptr;
    static inline BetterInventory* moduleInstance = nullptr;

    HoverRendererRenderHoverBoxHook();
    void enableHook() override;
    static void setModuleInstance(BetterInventory* instance) { moduleInstance = instance; }
};