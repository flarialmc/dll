
#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Actor/Actor.hpp"

class ItemDropEvent : public Event, public Cancellable{

    Actor *actor;
    ItemStack* item;
    bool DropItem;

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

    void setDropItem(bool dropItem)
    {
        this->DropItem = dropItem;
    }

    explicit ItemDropEvent(Actor *e, ItemStack* Item, bool Dropitem) {
        this->actor = e;
        this->item = Item;
        this->DropItem = Dropitem;
    }
};