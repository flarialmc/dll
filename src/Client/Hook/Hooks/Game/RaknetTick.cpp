#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include "ActorBaseTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include <filesystem>
#include <fstream>

//not updated
void RaknetTickHook::callback(RaknetConnector *raknet) {
    raknetTickOriginal(raknet);
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            RaknetTickEvent event{};
            EventHandler::onRaknetTick(event);
        }
    }
}


void RaknetTickHook::enableHook() {
    this->autoHook((void *) callback, (void **) &raknetTickOriginal);
}