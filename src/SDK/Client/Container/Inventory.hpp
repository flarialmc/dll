#pragma once

#include "../Item/ItemStack.hpp"

class Inventory {
public:
    BUILD_ACCESS(this, uintptr_t**, vtable, 0x0);
    ItemStack *getItem(int slot);
};

class SimpleContainer : public Inventory {}; // Derived from container but I CBA