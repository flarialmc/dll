#include "Mob.hpp"
#include "Actor.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void Mob::setSprinting(bool state) {
    static int off = GET_OFFSET("Mob::setSprinting");
    return Memory::CallVFuncI<void, bool>(off, this, state);
}

template<typename Component>
Component *Mob::tryGetComponent(uintptr_t addr) {
    auto a1 = **(uintptr_t ***) (this + 0x8);
    auto a2 = *(uintptr_t *) (this + 0x10);

    using efunc = Component *(__thiscall *)(uintptr_t, uintptr_t *);
    auto func = reinterpret_cast<efunc>(addr);
    return func(reinterpret_cast<uintptr_t>(a1), &a2);
}

MobBodyRotationComponent *Mob::getBodyRotationComponent() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Mob::getBodyRotationComponent"));
    }

    return tryGetComponent<MobBodyRotationComponent>(sig);
}