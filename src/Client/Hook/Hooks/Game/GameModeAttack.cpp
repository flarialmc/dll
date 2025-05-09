#include "GameModeAttack.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void GameModeAttackHook::callback(Gamemode *gamemode, Actor *actor) {
    //  Combo counter and reach counter logic will be done here in the next commit.
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->getPlayer()) {

            AttackEvent event(actor);
            EventHandler::onAttack(event);
        }

    }
    funcOriginal(gamemode, actor);
}

void GameModeAttackHook::callback1_21_50(Gamemode *gamemode, Actor *actor, bool a3) {
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->getPlayer()) {
            AttackEvent event(actor);
            EventHandler::onAttack(event);
        }
    }
    ((original1_21_50)funcOriginal)(gamemode, actor, a3);
}


GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack", GET_SIG("GameMode::attack")) {}


void GameModeAttackHook::enableHook() {
    if(WinrtUtils::checkAboveOrEqual(21, 50)) {
        static auto addr = GET_SIG_ADDRESS("GameMode::attack");
        this->manualHook((void *) addr, (void *) callback, (void **) &funcOriginal);
    } else {
        static auto base = GET_SIG_ADDRESS("GameMode::vtable");
        int offset = *reinterpret_cast<int *>(base + 3);
        auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

        static auto attackVftOffset = GET_OFFSET("Gamemode::attackVft");

        this->manualHook(vft[attackVftOffset], (void *) callback, (void **) &funcOriginal);
    }
}
