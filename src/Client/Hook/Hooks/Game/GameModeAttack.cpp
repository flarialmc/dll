#include "GameModeAttack.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"

void GameModeAttackHook::callback(Gamemode *gamemode, Actor *actor) {
    //  Combo counter and reach counter logic will be done here in the next commit.
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->player) {

            AttackEvent event(actor);
            EventHandler::onAttack(event);
        }

    }
    funcOriginal(gamemode, actor);
}


GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack",
                                                "48 8D 05 ? ? ? ? 48 89 01 48 89 51 08 48 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ?") {}


void GameModeAttackHook::enableHook() {
    auto base = Memory::findSig(this->signature);
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    this->manualHook(vft[14], (void *) callback, (void **) &funcOriginal);
}
