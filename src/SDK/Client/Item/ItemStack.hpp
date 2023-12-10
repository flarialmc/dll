#pragma once


#include "Item.hpp"
#include "../../src/Utils/Memory/Memory.hpp"

class ItemStack {
public:
	Item* getItem(void);
	BUILD_ACCESS(this, char, count, 0x22);
};