#include "Inventory.hpp"

ItemStack* getItem(int slot) {
	return Memory::CallVFunc<5, ItemStack*>(this, slot);
}