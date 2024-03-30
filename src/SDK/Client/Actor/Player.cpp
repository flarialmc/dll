#include "Player.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

float Player::getBreakprogress(Block block) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Player::getBreakprogress"));
    }

    auto fn = reinterpret_cast<float (__thiscall *)(Player *, Block)>(sig);
    return fn(this, block);
}