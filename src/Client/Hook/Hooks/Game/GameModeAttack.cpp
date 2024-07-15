#include "GameModeAttack.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

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


GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack", GET_SIG("GameMode::attack")) {}


void GameModeAttackHook::enableHook() {
    auto base = Memory::findSig(this->signature);
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    static auto attackVftOffset = GET_OFFSET("Gamemode::attackVft");

    this->manualHook(vft[attackVftOffset], (void *) callback, (void **) &funcOriginal);
}
