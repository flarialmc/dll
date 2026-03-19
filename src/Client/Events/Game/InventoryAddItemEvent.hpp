#pragma once

#include "../Event.hpp"
#include "../../../SDK/Client/Container/Inventory.hpp"

#include <string>

class InventoryAddItemEvent : public Event {
private:
    Inventory* inventory;
    ItemStack* itemStack;
    bool addedItem;
    std::string itemName;
    int itemCount = 0;

public:
    Inventory* getInventory() {
        return this->inventory;
    }

    ItemStack* getItemStack() {
        return this->itemStack;
    }

    bool getAddedItem() const {
        return this->addedItem;
    }

    const std::string& getItemName() const {
        return this->itemName;
    }

    int getItemCount() const {
        return this->itemCount;
    }

    explicit InventoryAddItemEvent(Inventory* inventory, ItemStack* itemStack, bool addedItem, const std::string& itemName = "", int itemCount = 0) {
        this->inventory = inventory;
        this->itemStack = itemStack;
        this->addedItem = addedItem;
        this->itemName = itemName;
        this->itemCount = itemCount;
    }
};
