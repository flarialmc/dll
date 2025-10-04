#include <fstream>
#include "ActorDropItem.hpp"
#include "../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Game/ItemDropEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "nes/event_dispatcher.hpp"
#include "SDK/SDK.hpp"
#include "Events/Events.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void ActorDropItem::enableHook() {

    auto base = address; // Player vtable
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    static auto vftOffset = GET_OFFSET("Actor::drop");

    this->manualHook(vft[vftOffset], (void *) callback, (void **) &funcOriginal);

}

ActorDropItem::ActorDropItem() : Hook("ActorDropItem", GET_SIG_ADDRESS("Actor::baseTick")) {}

bool ActorDropItem::callback(Actor *actor, ItemStack *item, const bool randomly) {

    bool DropItem = funcOriginal(actor, item, randomly);
    auto event = nes::make_holder<ItemDropEvent>(actor, item, DropItem);

    if (SDK::clientInstance)
    if (actor == SDK::clientInstance->getLocalPlayer()) {
        eventMgr.trigger(event);
    }

    return event->getDropItem();

}


