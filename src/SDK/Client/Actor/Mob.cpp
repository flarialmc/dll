#include "Mob.hpp"


void Mob::setSprinting(bool state) {
    Memory::CallVFunc<270, void, bool>(this, state);
}   