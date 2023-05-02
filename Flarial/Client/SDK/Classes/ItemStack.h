#pragma once
#include "Item.h"

class ItemStack {
public:
	auto getItem(void) -> Item* {
		return *(Item**)((uintptr_t)(this) + 0x8);
	};
	auto isBlockType(void) -> bool {
		return *(uintptr_t**)((uintptr_t)(this) + 0x18) != nullptr;
	};
public:
	AS_FIELD(Item*, item, getItem);
	AS_FIELD(bool, isBlock, isBlockType);
	BUILD_ACCESS(this, uint8_t, count, 0x22);
};