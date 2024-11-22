#include "UpdatePlayerHook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void UpdatePlayerHook::callback(void* a1, void* a2, void* a3) {
    //  Combo counter and reach counter logic will be done here in the next commit.
    UpdatePlayerEvent event;

    if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer() != nullptr) {
        EventHandler::onUpdatePlayer(event);
    }

    if(!event.isCancelled())
        funcOriginal(a1, a2, a3);
}


UpdatePlayerHook::UpdatePlayerHook() : Hook("UpdatePlayerHook", GET_SIG("_updatePlayer")) {}


void UpdatePlayerHook::enableHook() {
    this->autoHook((void *) callback, (void **) &funcOriginal);
}
