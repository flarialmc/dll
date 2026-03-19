#include "InventoryAddItem.hpp"

#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/InventoryAddItemEvent.hpp"

InventoryAddItem::InventoryAddItem() : Hook("InventoryAddItem", GET_SIG_ADDRESS("Inventory::addItem")) {}

bool InventoryAddItem::callback(Inventory* inventory, ItemStack& itemStack) {
    std::string itemName;
    const auto beforeItem = itemStack.getItem();
    if (beforeItem != nullptr) itemName = beforeItem->name;

    const int beforeCount = static_cast<int>(itemStack.count);

    const bool addedItem = funcOriginal(inventory, itemStack);

    if (itemName.empty()) {
        const auto afterItem = itemStack.getItem();
        if (afterItem != nullptr) itemName = afterItem->name;
    }

    const int afterCount = static_cast<int>(itemStack.count);
    int movedCount = beforeCount - afterCount;
    if (movedCount < 0) movedCount = 0;
    if (movedCount == 0 && addedItem) movedCount = beforeCount;

    auto event = nes::make_holder<InventoryAddItemEvent>(inventory, &itemStack, addedItem, itemName, movedCount);
    eventMgr.trigger(event);

    return addedItem;
}

void InventoryAddItem::enableHook() {
    this->autoHook((void*) callback, (void**) &funcOriginal);
}
