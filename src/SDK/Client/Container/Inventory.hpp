#pragma once

#include "../Item/ItemStack.hpp"

class Inventory {
public:
    ItemStack *getItem(int slot);
};

class SimpleContainer : public Inventory {}; // Derived from container but I CBA