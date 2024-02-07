#include "Player.hpp"

float Player::getBreakprogress(Block block) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 08 57 48 83 EC 50 48 8B ? ? ? ? ? 30");
    }

    auto fn = reinterpret_cast<float (__thiscall*)(Player*, Block)>(sig);
    return fn(this, block);
}

bool Player::canSee(Player const& actor) {


    using canSeeFunc = bool(__fastcall*)(Actor*, Player const&);
    static auto canSee = reinterpret_cast<canSeeFunc>(Memory::offsetFromSig(Memory::findSig("E8 ? ? ? ? 84 C0 74 1C 48 8B 4F 48"), 1));
    return canSee(this, actor);

    //return Memory::CallVFunc<82, bool, Player const&>(this, actor);
}