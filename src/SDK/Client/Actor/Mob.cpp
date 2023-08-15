#include "Mob.hpp"
#include "Components/MobBodyRotationComponent.hpp"
#include "Actor.hpp"

void Mob::setSprinting(bool state) {
    Memory::CallVFunc<256, void, bool>(this, state);
}

MobBodyRotationComponent* Mob::getBodyRotationComponent() {

    static  uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("40 53 48 83 EC 20 48 8B DA BA 2F B8 31 03");
    }

    return tryGet<MobBodyRotationComponent>(sig);
}