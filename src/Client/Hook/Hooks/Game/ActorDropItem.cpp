#include <fstream>
#include "ActorDropItem.hpp"
#include "../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/ItemDropEvent.hpp"
#include "nes/event_dispatcher.hpp"
#include "SDK/SDK.hpp"

void ActorDropItem::enableHook() {

    this->autoHook((void *) callback, (void **) &funcOriginal);

}

ActorDropItem::ActorDropItem() : Hook("ActorDropItem", GET_SIG_ADDRESS("Actor::drop")) {}

bool ActorDropItem::callback(Actor *actor, ItemStack *item, const bool randomly) {
    std::string itemName;
    int itemCount = 0;

    if (item != nullptr) {
        const auto droppedItem = item->getItem();
        if (droppedItem != nullptr) {
            itemName = droppedItem->name;
            itemCount = static_cast<int>(item->count);
        }
    }

    bool dropItem = funcOriginal(actor, item, randomly);

    if (SDK::clientInstance != nullptr && actor == SDK::clientInstance->getLocalPlayer()) {
        auto event = nes::make_holder<ItemDropEvent>(actor, item, dropItem, itemName, itemCount);
        eventMgr.trigger(event);
        return event->getDropItem();
    }

    return dropItem;

}


