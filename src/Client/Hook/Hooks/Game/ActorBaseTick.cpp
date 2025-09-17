#include <fstream>
#include "ActorBaseTick.hpp"
#include "../../../Client.hpp"
#include "Events/Game/TickEvent.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"

void ActorBaseTick::enableHook() {

    auto base = address; // Player vtable
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    static auto vftOffset = GET_OFFSET("Actor::baseTickVft");

    this->manualHook(vft[vftOffset], (void *) callback, (void **) &funcOriginal);

}

ActorBaseTick::ActorBaseTick() : Hook("ActorBaseTick", GET_SIG_ADDRESS("Actor::baseTick")) {}

void ActorBaseTick::callback(Actor *actor) {
    // TODO: Might be wrong, req checking
    funcOriginal(actor);
    if (Client::disable) return;
    if (actor != nullptr) {
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                if (actor == SDK::clientInstance->getLocalPlayer()) {
                    auto event = nes::make_holder<TickEvent>(actor);
                    eventMgr.trigger(event);
                }
            }
        }
    }
}


