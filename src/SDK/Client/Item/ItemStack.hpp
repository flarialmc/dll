#pragma once

#include "Item.hpp"
#include <SDK/Client/Core/Ref.hpp>
#include <Utils/Memory/Memory.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ItemStack {
public:
    uintptr_t** vtable; // 0x0
    WeakPtr<Item> item; // 0x8
    void* userData;     // 0x10, CompoundTag*
    class Block* block; // 0x18

    BUILD_ACCESS(this, void*, tag, GET_OFFSET("ItemStack::tag"));
    BUILD_ACCESS(this, unsigned char, count, GET_OFFSET("ItemStack::count"));

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] Item* getItem() const;
    short getDamageValue();
    short getMaxDamage();
    bool isEnchanted();
};