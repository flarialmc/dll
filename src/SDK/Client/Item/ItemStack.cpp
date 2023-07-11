#include "ItemStack.hpp"


Item* ItemStack::getItem() {
	auto itemlist = *(uintptr_t**)((uintptr_t)(this) + 0x8);
	if (itemlist != nullptr) {
		return *(Item**)((uintptr_t)(itemlist));
	}
	else {
		return nullptr;

	}
}