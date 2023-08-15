#include "Mob.hpp"
#include "Actor.hpp"

void Mob::setSprinting(bool state) {
    Memory::CallVFunc<256, void, bool>(this, state);
}