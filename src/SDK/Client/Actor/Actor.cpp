#include "Actor.hpp"
#include "Components/ActorGameTypeComponent.hpp"
#include "Components/AABBShapeComponent.hpp"
#include "Components/RuntimeIDComponent.hpp"
#include "Components/ActorDataFlagComponent.hpp"

// TODO add comments to all components, replace their sigs with simpler ones ?       marioCST: use entt's try_get func in EntityContext instead of using sigs, there are no simpler sigs

template<typename Component>
Component *Actor::tryGet(uintptr_t addr) {

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

int16_t Actor::getHurtTime() {
    return hat::member_at<int16_t>(this, GET_OFFSET("Actor::hurtTime"));
}

void Actor::setHurtTime(int16_t hurtTime) {
    int16_t& hurttime = hat::member_at<int16_t>(this, GET_OFFSET("Actor::hurtTime"));
    hurttime = hurtTime;
}

Level *Actor::getLevel() {
    return hat::member_at<Level*>(this, GET_OFFSET("Actor::level"));
}

ActorCategory Actor::getCategories() {
    return hat::member_at<ActorCategory>(this, GET_OFFSET("Actor::categories"));
}

bool Actor::canSee(const Actor& actor) {
    using canSeeFunc = bool (__fastcall *)(Actor *, const Actor&);
    static uintptr_t sig;
    if (sig == NULL) {
        if (!WinrtUtils::check(20, 40)) {
            sig = Memory::findSig(GET_SIG("Actor::canSee"));
        } else {
            sig = Memory::offsetFromSig(Memory::findSig(GET_SIG("Actor::canSee")), 1);
        }
    }
    static auto canSee = reinterpret_cast<canSeeFunc>(sig);
    return canSee(this, actor);
}

uint64_t Actor::getRuntimeID() {
    return this->GetEntityContextV1_20_50()->id;
}

ActorDataFlagComponent* Actor::getActorDataFlagComponent() {
    if(!WinrtUtils::check(20, 80)) return nullptr;

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorDataFlagComponent"));

    return tryGet<ActorDataFlagComponent>(sig);
}

bool Actor::getActorFlag(ActorFlags flag) {
    if(WinrtUtils::check(21, 20)) {
        auto actorDataFlagComponent = Actor::getActorDataFlagComponent();

        if(!actorDataFlagComponent) return false;

        return actorDataFlagComponent->flags[flag];
    }
    static int off = GET_OFFSET("Actor::getActorFlag");
    return Memory::CallVFuncI<bool, int>(off, this, flag);
}

Vec3<float> *Actor::getPosition() {
    return &this->getStateVectorComponent()->Pos;
}

SimpleContainer* Actor::getArmorContainer() {
    if(!WinrtUtils::check(20, 80)) return nullptr;

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorEquipmentComponent"));

    return tryGet<ActorEquipmentComponent>(sig)->mArmorContainer;
}

SimpleContainer* Actor::getOffhandContainer() {
    if(!WinrtUtils::check(20, 80)) return nullptr;

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorEquipmentComponent"));

    return tryGet<ActorEquipmentComponent>(sig)->mOffhandContainer;
}

ItemStack *Actor::getArmor(int slot) {
    if(WinrtUtils::check(20, 80)) {
        return Actor::getArmorContainer()->getItem(slot);
    } else {
        static uintptr_t sig;

        if (sig == NULL) {
            sig = Memory::findSig(GET_SIG("Actor::getArmor"));
        }

        auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *, int)>(sig);
        return fn(this, slot);
    }
}

MoveInputComponent *Actor::getMoveInputHandler() { //??$try_get@UMoveInputComponent

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getMoveInputHandler"));

    return tryGet<MoveInputComponent>(sig);
}

ActorGameTypeComponent *Actor::getGameModeType() {

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getActorGameTypeComponent"));

    return tryGet<ActorGameTypeComponent>(sig);
}

AABBShapeComponent *Actor::getAABBShapeComponent() {

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getAABBShapeComponent"));

    return tryGet<AABBShapeComponent>(sig);
}

StateVectorComponent *Actor::getStateVectorComponent() {

    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getStateVectorComponent"));

    return tryGet<StateVectorComponent>(sig);
}

ItemStack *Actor::getOffhandSlot() {
    if(WinrtUtils::check(20, 80)) {
        return getOffhandContainer()->getItem(0);
    } else {
        static uintptr_t sig;

        if (sig == NULL) {
            sig = Memory::findSig(GET_SIG("Actor::getOffhandSlot"));
        }

        auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *)>(sig);
        return fn(this);
    }
}

RuntimeIDComponent *Actor::getRuntimeIDComponent() {
    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getRuntimeIDComponent"));

    return tryGet<RuntimeIDComponent>(sig);
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
    return ((int) this->getCategories() & (int) category) != 0;
}

RenderPositionComponent *Actor::getRenderPositionComponent() { //??$try_get@URenderPositionComponent
    static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getRenderPositionComponent"));

    return tryGet<RenderPositionComponent>(sig);
}

bool Actor::isValidAABB() {
    auto AABBShapeComponent = this->getAABBShapeComponent();
    if(!AABBShapeComponent) return false;
    auto size = AABBShapeComponent->size;
    if(size.x < 0.1f || size.y < 0.1f) return false;
    return true;
}

bool Actor::isOnGround() {
    const auto ctx = this->GetEntityContextV1_20_50();

    if (WinrtUtils::check(20, 60)) {
        using isOnGroundFunc = bool(__fastcall *)(uintptr_t&, EntityId&);
        static isOnGroundFunc isOnGround = Memory::getOffsetFromSig<isOnGroundFunc>(Memory::findSig(GET_SIG("ActorCollision::isOnGround")), 1);

        if (isOnGround)
            return isOnGround(ctx->basicReg, ctx->id);

        return false;
    }

    using isOnGroundFunc = bool(__fastcall *)(V1_20_50::EntityContext*);
    static isOnGroundFunc isOnGround = reinterpret_cast<isOnGroundFunc>(Memory::findSig(GET_SIG("ActorCollision::isOnGround")));

    if (isOnGround)
        return isOnGround(ctx);

    return false;
}
