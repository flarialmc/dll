#pragma once

#include "../Item/ItemStack.h"

class Inventory {
public:
    ItemStack* getItem(int slot) {
        return Memory::CallVFunc<5, ItemStack*>(this, slot);
    }
};