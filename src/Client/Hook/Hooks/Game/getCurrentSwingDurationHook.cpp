#include "getCurrentSwingDurationHook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/Game/SwingDurationEvent.hpp"
#include "../../../Events/EventManager.hpp"
#include "Events/Events.hpp"

int getCurrentSwingDurationHook::callback(void* mob) {
    int originalDuration = funcOriginal(mob);

    auto event = nes::make_holder<SwingDurationEvent>(originalDuration);
    eventMgr.trigger(event);
    return event->getDuration();
}

getCurrentSwingDurationHook::getCurrentSwingDurationHook() : Hook("getCurrentSwingDurationHook", GET_SIG_ADDRESS("Mob::getCurrentSwingDuration")) {}

void getCurrentSwingDurationHook::enableHook() {
    this->autoHook((void *) callback, (void **) &funcOriginal);
}
