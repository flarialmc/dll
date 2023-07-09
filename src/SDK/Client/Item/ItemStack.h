#pragma once


#include "Item.h"
#include "../../src/Utils/Memory/Memory.hpp"

class ItemStack {
public:
	Item* getItem(void) {
		auto itemlist = *(uintptr_t**)((uintptr_t)(this) + 0x8);
		if (itemlist != nullptr) {
			return *(Item**)((uintptr_t)(itemlist));
		}
		else {
			return nullptr;

		} 
	};

	BUILD_ACCESS(this, int, count, 0x20);
};