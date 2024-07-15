#include "Inventory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

ItemStack *Inventory::getItem(int slot) {
    static int off = GET_OFFSET("Inventory::getItem");
    return Memory::CallVFuncI<ItemStack *>(off, this, slot);
}