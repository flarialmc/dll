#pragma once

#include "../../src/Utils/Memory/Memory.hpp"
#include "Inventory.hpp"

class PlayerInventory {
public:
	BUILD_ACCESS(this, Inventory*, inventory, 0xC0);
};