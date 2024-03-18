#include "Actor.hpp"

// TODO add comments to all components, replace their sigs with simpler ones ?

template<typename Component>
Component *Actor::tryGet(uintptr_t addr) {
    auto ctx = GetEntityContext();

    using efunc = Component *(__thiscall *)(uintptr_t *, const EntityId &);
    auto func = reinterpret_cast<efunc>(addr);
    return func(&ctx->basicReg, ctx->id);
}

bool Actor::isAlive() {
    return Memory::CallVFunc<53, bool>(this);
}

std::string *Actor::getXuid(std::string *str) {
    return Memory::CallVFunc<253, std::string *, std::string *>(this, str);
}

ActorTypeComponent *Actor::getActorTypeComponent() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 8B 89 20 04 00 00 48 8B 01 48 8B 40 28 48");
    }

    return tryGet<ActorTypeComponent>(sig);
}


int Actor::getEntityTypeId() {
    return getActorTypeComponent()->type;
}

bool Actor::canSee(const Actor& actor) {
    using canSeeFunc = bool (__fastcall *)(Actor *, const Actor&);
    static auto canSee = reinterpret_cast<canSeeFunc>(Memory::offsetFromSig(
            Memory::findSig("E8 ? ? ? ? 84 C0 74 1C 48 8B 4F 48"), 1));
    return canSee(this, actor);

    //return Memory::CallVFunc<82, bool, Player const&>(this, actor);
}

MobEffectInstance *Actor::getEffect(MobEffect *effect) {
    static uintptr_t addr = Memory::findSig("E8 ? ? ? ? 8B 78 14");
    static auto realAddr = addr + 1 + 4 + *reinterpret_cast<int *>(addr + 1);
    auto fn = reinterpret_cast<MobEffectInstance *(__cdecl *)(Actor *, MobEffect *)>(realAddr);
    return fn(this, effect);
}

bool Actor::getActorFlag(int flag) {
    return Memory::CallVFunc<0, bool, int>(this, flag);
}

Vec3<float> *Actor::getPosition() {
    return &this->stateVector->Pos;
}

ItemStack *Actor::getArmor(int slot) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 8B 89 20 04 00 00 48 8B 01 48 8B 40 28 48");
    }

    auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *, int)>(sig);
    return fn(this, slot);
}

MoveInputComponent *Actor::getMoveInputHandler() { //??$try_get@UMoveInputComponent

    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 ? 57 48 83 EC ? 48 8B DA BA 2E CD 8B 46");
    }

    return tryGet<MoveInputComponent>(sig);

}

ActorHeadRotationComponent *Actor::getActorHeadRotationComponent() { //??$try_get@UActorHeadRotationComponent

    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 ? 57 48 83 EC ? 48 8B DA BA 1C 58 40 E9");
    }

    return tryGet<ActorHeadRotationComponent>(sig);

}

ItemStack *Actor::getOffhandSlot() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 8B 89 ? ? ? ? BA ? ? ? ? 48 8B 01 48 8B 40 ? 48 FF 25");
    }

    auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *)>(sig);
    return fn(this);
}

EntityContext *Actor::GetEntityContext() {
    auto address = reinterpret_cast<uintptr_t>(this);
    return reinterpret_cast<EntityContext *>(address + 8);
}

void Actor::setNametag(std::string *name) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84");
    }

    auto fn = reinterpret_cast<void (__thiscall *)(Actor *, std::string *)>(sig);
    return fn(this, name);
}

/*
std::uint64_t Actor::getRuntimeID() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("40 53 48 83 EC 20 8B 41 18 48 8B DA 48 8B 49 10 48 8D 54 24 30 89 44 24 30 E8 ?? ?? ?? ?? 48 85 C0 74 0F");
    }

    auto fn = reinterpret_cast<std::uint64_t (__thiscall*)(Actor*)>(sig);
    return fn(this);
}
*/

std::string *Actor::getNametag() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(
                "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 74 3B 48 8B 08 BA ? ? ? ? 48 8B 40 ? 48 2B C1 48 C1 F8 ? 66 3B D0 73 17");
    }

    auto fn = reinterpret_cast<std::string *(__thiscall *)(Actor *)>(sig);
    return fn(this);
}

bool Actor::isPlayer() {
    return Memory::CallVFunc<58, bool>(this);
}

bool Actor::hasCategory(ActorCategory category) {
    return ((int) this->categories & (int) category) != 0;
}

RenderPositionComponent *Actor::getRenderPositionComponent() { //??$try_get@URenderPositionComponent
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig("48 89 5C 24 ? 57 48 83 EC ? 48 8B DA BA 6E F3 E8 D4");
    }

    return tryGet<RenderPositionComponent>(sig);
}

bool Actor::isValidTarget(Actor *actor) {
    return Memory::CallVFunc<61, bool, Actor *>(this, actor);
}
