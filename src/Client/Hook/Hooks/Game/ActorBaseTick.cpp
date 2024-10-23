#include <fstream>
#include "ActorBaseTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"

void ActorBaseTick::enableHook() {

    auto vft = Memory::getOffsetFromSig<uintptr_t*>(Memory::findSig(this->signature), 3); // Player vtable
    static auto vftOffset = GET_OFFSET("Actor::baseTickVft");

    this->manualHook((void*)vft[vftOffset], (void *) callback, (void **) &funcOriginal);

}

ActorBaseTick::ActorBaseTick() : Hook("ActorBaseTick", GET_SIG("Actor::baseTick")) {}

void ActorBaseTick::callback(Actor *actor) {
    // TODO: Might be wrong, req checking
    funcOriginal(actor);
    if (Client::disable) return;
    if (actor != nullptr) {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (actor == SDK::clientInstance->getLocalPlayer()) {
                    TickEvent event(actor);
                    EventHandler::onTick(event);
                }
            }
        }
    }
}


