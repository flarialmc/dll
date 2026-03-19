
#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Actor/Actor.hpp"
#include <string>

class ItemDropEvent : public Event, public Cancellable{

    Actor *actor;
    ItemStack* item;
    bool DropItem;
    std::string itemName;
    int itemCount = 0;

public:
    Actor *getActor() {
        return this->actor;
    }
    ItemStack *getItem()
    {
        return this->item;
    }

    bool getDropItem()
    {
        return this->DropItem;
    }

    const std::string& getItemName() const {
        return this->itemName;
    }

    int getItemCount() const {
        return this->itemCount;
    }

    void setDropItem(bool dropItem)
    {
        this->DropItem = dropItem;
    }

    explicit ItemDropEvent(Actor *e, ItemStack* Item, bool Dropitem, const std::string& itemName = "", int itemCount = 0) {
        this->actor = e;
        this->item = Item;
        this->DropItem = Dropitem;
        this->itemName = itemName;
        this->itemCount = itemCount;
    }
};
