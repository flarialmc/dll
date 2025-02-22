#include "GameModeAttack.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void GameModeAttackHook::callback(Gamemode *gamemode, Actor *actor) {
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->getPlayer()) {
            auto event = nes::make_holder<AttackEvent>(actor);
            eventMgr.trigger(event);
        }

    }
    funcOriginal(gamemode, actor);
}


GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack", GET_SIG_ADDRESS("GameMode::vtable")) {}


void GameModeAttackHook::enableHook() {
    auto base = address;
    int offset = *reinterpret_cast<int *>(base + 3);
    auto **vft = reinterpret_cast<uintptr_t **>(base + offset + 7);

    static auto attackVftOffset = GET_OFFSET("Gamemode::attackVft");

    this->manualHook(vft[attackVftOffset], (void *) callback, (void **) &funcOriginal);
}
