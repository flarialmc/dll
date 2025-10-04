#include "GameModeAttack.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/Game/AttackEvent.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"

void GameModeAttackHook::callback(Gamemode *gamemode, Actor *actor) {
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->getPlayer()) {
            auto event = nes::make_holder<AttackEvent>(actor);
            eventMgr.trigger(event);
        }

    }
    ((original)funcOriginal)(gamemode, actor);
}

void GameModeAttackHook::callback1_21_50(Gamemode *gamemode, Actor *actor, bool a3) {
    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() == gamemode->getPlayer()) {
            auto event = nes::make_holder<AttackEvent>(actor);
            eventMgr.trigger(event);
        }
    }
    ((original1_21_50)funcOriginal)(gamemode, actor, a3);
}


GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack", 0) {}


void GameModeAttackHook::enableHook() {
    if(VersionUtils::checkAboveOrEqual(21, 50)) {
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
