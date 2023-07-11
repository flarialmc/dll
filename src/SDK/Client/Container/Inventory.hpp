#pragma once

#include "../Item/ItemStack.hpp"

class Inventory {
public:
    ItemStack* getItem(int slot);
};