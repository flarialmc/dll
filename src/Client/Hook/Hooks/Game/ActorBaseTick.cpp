#include <filesystem>
#include <fstream>
#include "ActorBaseTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "RaknetTick.hpp"
#include "../../../Client.hpp"

void ActorBaseTick::enableHook() {

    auto base = Memory::findSig(this->signature); // Player vtable
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    this->manualHook(vft[29], (void *) callback, (void **) &funcOriginal);

}

ActorBaseTick::ActorBaseTick() : Hook("ActorBaseTick",
                                      "48 8D 05 ? ? ? ? 48 89 01 B8 ? ? ? ? 8D 50 FA 44 8D 48 ? 44 8D 40 ? 66 89 44 ? ? E8 ? ? ? ? 48 8B 8B") {}

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


