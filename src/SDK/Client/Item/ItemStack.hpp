#pragma once

#include "Item.hpp"
#include <SDK/Client/Core/Ref.hpp>
#include <Utils/Memory/Memory.hpp>

// Forward declarations
class CompoundTag;
class BlockLegacy;

// Member names match LeviLamina's ItemStackBase layout.
// Offsets 0x0–0x18 are stable across all supported versions.
// Fields after 0x18 use BUILD_ACCESS with version-specific offsets.
class ItemStack {
public:
    uintptr_t** vtable;        // 0x0
    WeakPtr<Item> mItem;       // 0x8
    CompoundTag* mUserData;    // 0x10 (LeviLamina: std::unique_ptr<CompoundTag>)
    class Block* mBlock;       // 0x18

    BUILD_ACCESS(this, short, auxValue, GET_OFFSET("ItemStack::auxValue"));
    BUILD_ACCESS(this, unsigned char, count, GET_OFFSET("ItemStack::count"));
    BUILD_ACCESS(this, bool, valid, GET_OFFSET("ItemStack::valid"));
    BUILD_ACCESS(this, bool, showPickUp, GET_OFFSET("ItemStack::showPickUp"));
    BUILD_ACCESS(this, uint64_t, pickupTime, GET_OFFSET("ItemStack::pickupTime"));

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] Item* getItem() const;
    short getDamageValue();
    short getMaxDamage();
    bool isEnchanted();

    // Load item data from NBT compound tag
    // Note: Requires "ItemStack::_loadItem" signature to be defined
    void loadItem(const CompoundTag& tag);

    // Full ItemStack initialization path used by the game constructor.
    // Calls ItemStackBase::_loadComponents which internally runs _loadItem
    // and item-specific component initialization.
    bool loadComponents(const CompoundTag& tag);

    // For block items, initialize ItemStack fields from BlockLegacy.
    // Uses ItemStack::_init_fromBlock (sets Item WeakPtr, timestamp, etc.).
    bool initFromBlock(BlockLegacy* legacy);

    // Retrieve UV float array for this item's atlas sprite.
    // Returns a pointer to a 5-element float array: {u0, v0, u1, v1, ?}.
    // The 'frame' and 'aux' parameters mirror the game's internal call convention.
    // Pass frame=0, aux=0 for the default icon.
    float* getTextureAtlasSprite(int frame, int aux);
};
