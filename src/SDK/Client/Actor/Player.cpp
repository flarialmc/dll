#include "Player.hpp"

float Player::getBreakprogress(Block block) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 08 57 48 83 EC 50 48 8B F9 0F 29 7C 24 30");
    }

    auto fn = reinterpret_cast<float (__thiscall*)(Player*, Block)>(sig);
    return fn(this, block);
}