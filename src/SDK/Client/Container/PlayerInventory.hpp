#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Inventory.hpp"

class PlayerInventory {
public:
    BUILD_ACCESS(this, int, SelectedSlot, GET_OFFSET("PlayerInventory::SelectedSlot"));

    BUILD_ACCESS(this, Inventory*, inventory, GET_OFFSET("PlayerInventory::inventory"));
};