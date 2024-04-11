#include <filesystem>
#include <fstream>
#include "ActorBaseTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "RaknetTick.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void ActorBaseTick::enableHook() {

    auto base = Memory::findSig(this->signature); // Player vtable
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    static auto vftOffset = GET_OFFSET("Actor::baseTickVft");

    this->manualHook(vft[vftOffset], (void *) callback, (void **) &funcOriginal);

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


