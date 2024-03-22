#pragma once


#include "Item.hpp"
#include "../Core/Ref.hpp"
#include "../../src/Utils/Memory/Memory.hpp"

class ItemStack {
public:
    uintptr_t** vtable; // 0x0
    WeakPtr<Item> item; // 0x8

    BUILD_ACCESS(this, void*, tag, 0x10);
    BUILD_ACCESS(this, char, count, 0x22);

    bool isValid();
    Item* getItem();
    short getDamageValue();
    short getMaxDamage();
};