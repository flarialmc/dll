#include "GameModeAttack.hpp"


void GameModeAttackHook::callback(Gamemode* gamemode, Actor* actor) {


    //  Combo counter and reach counter logic will be done here in the next commit.

    attackOriginal(gamemode, actor);
}




GameModeAttackHook::GameModeAttackHook() : Hook("GameModeAttack", "48 8D 05 ? ? ? ? 48 89 01 48 89 51 08 48 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ?") {}


void GameModeAttackHook::enableHook() {


    auto vft = Memory::findSig("48 8D 05 ? ? ? ? 48 89 01 48 89 51 08 48 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ?");
    int xd = *reinterpret_cast<int*>(vft + 3);
    uintptr_t** vftREAL = reinterpret_cast<uintptr_t**>(vft + xd + 7);

    this->manualHook(vftREAL[14], callback, (void**)&attackOriginal);
}
