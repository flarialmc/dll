#include "Inventory.hpp"

ItemStack* Inventory::getItem(int slot) {
	return Memory::CallVFunc<5, ItemStack*>(this, slot);
}