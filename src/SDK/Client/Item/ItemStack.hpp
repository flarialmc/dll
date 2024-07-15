#pragma once


#include "Item.hpp"
#include "../Core/Ref.hpp"
#include "../../src/Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ItemStack {
public:
    uintptr_t** vtable; // 0x0
    WeakPtr<Item> item; // 0x8

    BUILD_ACCESS(this, void*, tag, GET_OFFSET("ItemStack::tag"));
    BUILD_ACCESS(this, char, count, GET_OFFSET("ItemStack::count"));

    bool isValid();
    Item* getItem();
    short getDamageValue();
    short getMaxDamage();
};