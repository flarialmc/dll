#include "Actor.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

// TODO add comments to all components, replace their sigs with simpler ones ?       marioCST: use entt's try_get func in EntityContext instead of using sigs, there are no simpler sigs

template<typename Component>
Component *Actor::tryGet(uintptr_t addr) { // TODO: Multiversion

    uintptr_t* basicReg;
    EntityId id;

    if(WinrtUtils::check(20, 50)) {
        auto ctx = GetEntityContextV1_20_50();
        id = ctx->id;
        basicReg = &ctx->basicReg;
        using efunc = Component *(__thiscall *)(uintptr_t *, const EntityId &);
        auto func = reinterpret_cast<efunc>(addr);
        return func(basicReg, id);
    }else{
        auto a1 = **(uintptr_t***)(this + 0x8);
        auto a2 = *(uintptr_t*)(this + 0x10);

        using efunc = Component* (__thiscall*)(uintptr_t, uintptr_t*);
        auto func = reinterpret_cast<efunc>(addr);
        return func(reinterpret_cast<uintptr_t>(a1), &a2);
    }
}

bool Actor::isAlive() {
    static int off = GET_OFFSET("Actor::isAlive");
    return Memory::CallVFuncI<bool>(off, this);
}

std::string *Actor::getXuid(std::string *str) {
    static int off = GET_OFFSET("Player::getXuid");
    return Memory::CallVFuncI<std::string *, std::string *>(off, this, str);
}

bool Actor::canSee(const Actor& actor) {
    using canSeeFunc = bool (__fastcall *)(Actor *, const Actor&);
    static auto canSee = reinterpret_cast<canSeeFunc>(Memory::offsetFromSig(
            Memory::findSig(GET_SIG("Actor::canSee")), 1));
    return canSee(this, actor);
}

MobEffectInstance *Actor::getEffect(MobEffect *effect) {
    static uintptr_t addr = Memory::findSig(GET_SIG("Actor::getEffect"));
    static auto realAddr = addr + 1 + 4 + *reinterpret_cast<int *>(addr + 1);
    auto fn = reinterpret_cast<MobEffectInstance *(__cdecl *)(Actor *, MobEffect *)>(realAddr);
    return fn(this, effect);
}

bool Actor::getActorFlag(int flag) {
    static int off = GET_OFFSET("Actor::getActorFlag");
    return Memory::CallVFuncI<bool, int>(off, this, flag);
}

Vec3<float> *Actor::getPosition() {
    return &this->stateVector->Pos;
}

ItemStack *Actor::getArmor(int slot) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::getArmor"));
    }

    auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *, int)>(sig);
    return fn(this, slot);
}

MoveInputComponent *Actor::getMoveInputHandler() { //??$try_get@UMoveInputComponent

    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::getMoveInputHandler"));
    }

    return tryGet<MoveInputComponent>(sig);

}

ItemStack *Actor::getOffhandSlot() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::getOffhandSlot"));
    }

    auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *)>(sig);
    return fn(this);
}

V1_20_50::EntityContext *Actor::GetEntityContextV1_20_50() {
    return reinterpret_cast<V1_20_50::EntityContext*>((uintptr_t)this + 0x8);
}

void Actor::setNametag(std::string *name) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::setNametag"));
    }

    auto fn = reinterpret_cast<void (__thiscall *)(Actor *, std::string *)>(sig);
    return fn(this, name);
}

std::string *Actor::getNametag() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::getNametag"));
    }

    auto fn = reinterpret_cast<std::string *(__thiscall *)(Actor *)>(sig);
    return fn(this);
}

bool Actor::hasCategory(ActorCategory category) {
    return ((int) this->categories & (int) category) != 0;
}

RenderPositionComponent *Actor::getRenderPositionComponent() { //??$try_get@URenderPositionComponent
    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("Actor::getRenderPositionComponent"));
    }

    return tryGet<RenderPositionComponent>(sig);
}

bool Actor::isValidTarget(Actor *actor) {
    static int off = GET_OFFSET("Actor::isValidTarget");
    return Memory::CallVFuncI<bool, Actor *>(off, this, actor);
}
