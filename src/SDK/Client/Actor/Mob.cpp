#include "Mob.hpp"


void Mob::setSprinting(bool state) {
    Memory::CallVFunc<256, void, bool>(this, state);
}   