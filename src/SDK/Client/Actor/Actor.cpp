#include "Actor.hpp"

template <typename Component>
Component* Actor::tryGet(uintptr_t addr) {
    auto a1 = **(uintptr_t***)(this + 0x8);
    auto a2 = *(uintptr_t*)(this + 0x10);

    using efunc = Component* (__thiscall*)(uintptr_t, uintptr_t*);
    auto func = reinterpret_cast<efunc>(addr);
    return func(reinterpret_cast<uintptr_t>(a1), &a2);
}

bool Actor::isAlive() {
    return Memory::CallVFunc<89, bool>(this);
}

std::string* Actor::getXuid(std::string* str) {
    return Memory::CallVFunc<396, std::string*, std::string*>(this, str);
}

int Actor::getEntityTypeId() {
    return Memory::CallVFunc<151, int>(this);
}

bool Actor::getActorFlag(int flag) {
    return Memory::CallVFunc<0, bool, int>(this, flag);
}

Vec3<float>* Actor::getPosition() {
    return Memory::CallVFunc<22, Vec3<float>*>(this);
}

bool Actor::wasHurt() {
    return Memory::CallVFunc<211, bool>(this);
}

ItemStack* Actor::getArmor(int slot) {
    return Memory::CallVFunc<139, ItemStack*, int>(this, slot);
}

ActorMovementProxyComponent* Actor::getMovementProxyComponent() {
    static  uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("40 53 48 83 EC 20 48 8B DA BA 18 0C BD EC");
    }

    return tryGet<ActorMovementProxyComponent>(sig);

}

MoveInputComponent* Actor::getMoveInputHandler() {

    static  uintptr_t sig;

    if (sig == NULL) {
       sig = Memory::findSig("40 53 48 83 EC 20 48 8B DA BA 2E");
    }

    return tryGet<MoveInputComponent>(sig);

}

ActorHeadRotationComponent* Actor::getActorHeadRotationComponent() {

    static  uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("40 53 48 83 EC 20 48 8B DA BA 1C 58 40 E9");
    }

    return tryGet<ActorHeadRotationComponent>(sig);

}

ItemStack* Actor::getOffhandSlot() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 8B 89 30 04 00 00 BA 01 00 00 00");
    }

    auto fn = reinterpret_cast<ItemStack * (__thiscall*)(Actor*)>(sig);
    return fn(this);
}

EntityContext* Actor::GetEntityContext() {
    uintptr_t address = reinterpret_cast<uintptr_t>(this);
    return reinterpret_cast<EntityContext*>(address + 8);
}

void Actor::setNametag(std::string* name) {
    Memory::CallVFunc<55, void>(this, name);
}

std::string* Actor::getNametag() {
    return Memory::CallVFunc<56, std::string*>(this);
}