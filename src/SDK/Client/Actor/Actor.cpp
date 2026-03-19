#include "Actor.hpp"
#include "Components/ActorGameTypeComponent.hpp"
#include "Components/AABBShapeComponent.hpp"
#include "Components/RuntimeIDComponent.hpp"
#include "Components/ActorDataFlagComponent.hpp"
#include "Components/ActorUniqueIDComponent.hpp"
#include "Components/ActorDefinitionIdentifierComponent.hpp"
#include "Components/MobBodyRotationComponent.hpp"
#include "../../SDK.hpp"
#include "../../../Client/GUI/Engine/Engine.hpp"
#include "Components/OnGroundFlagComponent.hpp"
#include "EntityContext.hpp"

template<typename Component>
Component *Actor::tryGet(uintptr_t addr) {
    if (VersionUtils::checkAboveOrEqual(21, 00) || addr == 0) {
        auto& ctx = GetEntityContextV1_20_50();
        if(!ctx.isValid()) return nullptr;
        Component* component = ctx.tryGetComponent<Component>();
        return component;
    }
    return tryGetOld<Component>(addr);
}

template<typename Component>
Component *Actor::tryGetOld(uintptr_t addr) {
    if (VersionUtils::checkAboveOrEqual(20, 50)) {
        auto ctx = GetEntityContextV1_20_50();
        EntityId id = ctx.entity;
        using efunc = Component *(__thiscall *)(entt::basic_registry<EntityId>&, const EntityId &);
        auto func = reinterpret_cast<efunc>(addr);
        return func(ctx.enttRegistry, id);
    } else {
        uintptr_t* basicReg = **(uintptr_t***)(this + 0x8);
        uint32_t id = *(uintptr_t*)(this + 0x10);

        using efunc = Component* (__thiscall*)(uintptr_t, uint32_t*);
        auto func = reinterpret_cast<efunc>(addr);
        return func(reinterpret_cast<uintptr_t>(basicReg), &id);
    }
}

template<typename Component>
bool Actor::hasComponent(uintptr_t addr) {
    if (VersionUtils::checkAboveOrEqual(21, 00) || addr == 0) {
        auto ctx = this->GetEntityContextV1_20_50();
        return ctx.hasComponent<Component>();
    }
    return tryGetOld<Component>(addr) != nullptr;
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

std::string Actor::getAlias() {
    return hat::member_at<std::string>(this, GET_OFFSET("Actor::mAlias"));
}

ActorCategory Actor::getCategories() {
    return hat::member_at<ActorCategory>(this, GET_OFFSET("Actor::categories"));
}

bool Actor::canSee(const Actor& actor) {
    using canSeeFunc = bool (__fastcall *)(Actor *, const Actor&);
    static uintptr_t sig;
    if (sig == NULL) {
        if (!VersionUtils::checkAboveOrEqual(20, 40)) {
            sig = GET_SIG_ADDRESS("Actor::canSee");
        } else {
            sig = Memory::offsetFromSig(GET_SIG_ADDRESS("Actor::canSee"), 1);
        }
    }
    static auto canSee = reinterpret_cast<canSeeFunc>(sig);
    return canSee(this, actor);
}

ActorDataFlagComponent* Actor::getActorDataFlagComponent() {
    if (!VersionUtils::checkAboveOrEqual(20, 80)) return nullptr;
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorDataFlagComponent"), "Actor::getActorDataFlagComponent");
        }
    }

    return tryGet<ActorDataFlagComponent>(sig);
}

bool Actor::getActorFlag(ActorFlags flag) {
    if(VersionUtils::checkAboveOrEqual(21, 20)) {
        auto actorDataFlagComponent = Actor::getActorDataFlagComponent();

        if(!actorDataFlagComponent) return false;

        return actorDataFlagComponent->flags[flag];
    }
    static int off = GET_OFFSET("Actor::getActorFlag");
    return Memory::CallVFuncI<bool, int>(off, this, flag);
}

Vec3<float> *Actor::getPosition() {
    auto* stateVec = this->getStateVectorComponent();
    if (!stateVec) return nullptr;
    return &stateVec->Pos;
}

SimpleContainer* Actor::getArmorContainer() {
    if(!VersionUtils::checkAboveOrEqual(20, 80)) return nullptr;

    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorEquipmentComponent"), "Actor::getActorEquipmentComponent (armor)");
        }
    }

    auto* equip = tryGet<ActorEquipmentComponent>(sig);
    return equip ? equip->mArmorContainer : nullptr;
}

SimpleContainer* Actor::getOffhandContainer() {
    if(!VersionUtils::checkAboveOrEqual(20, 80)) return nullptr;

    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getActorEquipmentComponent"), "Actor::getActorEquipmentComponent (offhand)");
        }
    }

    auto* equip = tryGet<ActorEquipmentComponent>(sig);
    return equip ? equip->mOffhandContainer : nullptr;
}

ItemStack *Actor::getArmor(int slot) {
    if(VersionUtils::checkAboveOrEqual(20, 80)) {
        auto* container = Actor::getArmorContainer();
        return container ? container->getItem(slot) : nullptr;
    } else {
        static uintptr_t sig;

        if (sig == NULL) {
            sig = GET_SIG_ADDRESS("Actor::getArmor");
        }

        auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *, int)>(sig);
        return fn(this, slot);
    }
}

MoveInputComponent *Actor::getMoveInputHandler() { //??$try_get@UMoveInputComponent
    return tryGet<MoveInputComponent>(0);
}

MoveInputComponentOLD *Actor::getMoveInputHandlerOLD() { //??$try_get@UMoveInputComponent
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getMoveInputHandler"), "Actor::getMoveInputHandler");
        }
    }

    return tryGet<MoveInputComponentOLD>(sig);
}

ActorGameTypeComponent *Actor::getGameModeType() {
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getActorGameTypeComponent"), "Actor::getActorGameTypeComponent");
        }
    }

    return tryGet<ActorGameTypeComponent>(sig);
}

AABBShapeComponent *Actor::getAABBShapeComponent() {
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getAABBShapeComponent"), "Actor::getAABBShapeComponent");
        }
    }
    return tryGet<AABBShapeComponent>(sig);
}

StateVectorComponent *Actor::getStateVectorComponent() {
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getStateVectorComponent"), "Actor::getStateVectorComponent");
        }
    }
    return tryGet<StateVectorComponent>(sig);
}

ActorRotationComponent *Actor::getActorRotationComponent() {
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getActorRotationComponent"), "Actor::getActorRotationComponent");
        }
    }

    return tryGet<ActorRotationComponent>(sig);
}

SynchedActorDataComponent* Actor::getSynchedActorDataComponent() {
	static uintptr_t sig;

	if (!VersionUtils::checkAboveOrEqual(21, 00)) {
		if (sig == NULL) {
			sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getSynchedActorDataComponent"), "Actor::getSynchedActorDataComponent");
		}
	}

	return tryGet<SynchedActorDataComponent>(sig);
}

ItemStack *Actor::getOffhandSlot() {
    if(VersionUtils::checkAboveOrEqual(20, 80)) {
        auto* container = getOffhandContainer();
        return container ? container->getItem(1) : nullptr;
    } else {
        static uintptr_t sig;

        if (sig == NULL) {
            sig = GET_SIG_ADDRESS("Actor::getOffhandSlot");
        }

        auto fn = reinterpret_cast<ItemStack *(__thiscall *)(Actor *)>(sig);
        return fn(this);
    }
}

RuntimeIDComponent *Actor::getRuntimeIDComponent() {
    static uintptr_t sig;
    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            if (VersionUtils::checkAboveOrEqual(20, 50) && !VersionUtils::checkAboveOrEqual(20, 60)) {
                sig = Memory::findSig(
                        std::string(GET_SIG("tryGetPrefix2")) + " " + GET_SIG("Actor::getRuntimeIDComponent"), "Actor::getRuntimeIDComponent");
            } else {
                sig = Memory::findSig(
                        std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getRuntimeIDComponent"), "Actor::getRuntimeIDComponent");
            }
        }
    }
    return tryGet<RuntimeIDComponent>(sig);
}

V1_20_50::EntityContext &Actor::GetEntityContextV1_20_50() {
    return hat::member_at<V1_20_50::EntityContext>(this, 0x8);
}

void Actor::setNametag(std::string *name) {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = GET_SIG_ADDRESS("Actor::setNametag");
    }

    auto fn = reinterpret_cast<void (__thiscall *)(Actor *, std::string *)>(sig);
    return fn(this, name);
}

std::string *Actor::getNametag() {
    static uintptr_t sig;

    if (sig == NULL) {
        sig = GET_SIG_ADDRESS("Actor::getNametag");
    }

    auto fn = reinterpret_cast<std::string *(__thiscall *)(Actor *)>(sig);
    return fn(this);
}

bool Actor::hasCategory(ActorCategory category) {
    return ((int) this->getCategories() & (int) category) != 0;
}

bool Actor::isValid() {
    auto lp = SDK::clientInstance->getLocalPlayer();
    if(!lp) return false;
    auto level = lp->getLevel();
    if(!level) return false;
    //if(!this->GetEntityContextV1_20_50().isValid()) return false;
    return true;
}

RenderPositionComponent *Actor::getRenderPositionComponent() { //??$try_get@URenderPositionComponent
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getRenderPositionComponent"), "Actor::getRenderPositionComponent");
        }
    }

    return tryGet<RenderPositionComponent>(sig);
}

std::vector<UnifiedMobEffectData> Actor::getMobEffects() {
    static uintptr_t sig;

    if(!VersionUtils::checkAboveOrEqual(21, 00)) {
        if (sig == NULL) {
            sig = Memory::findSig(
                    std::string(GET_SIG("tryGetPrefix")) + " " + GET_SIG("Actor::getMobEffectsComponent"), "Actor::getMobEffectsComponent");
        }
    }
    std::vector<UnifiedMobEffectData> unifiedEffects;
    auto component =  tryGet<MobEffectsComponent>(sig);
    if(component) {
        unifiedEffects = component->getUnifiedEffects();
    }
    return unifiedEffects;
}

bool Actor::isValidAABB() {
    auto AABBShapeComponent = this->getAABBShapeComponent();
    if(!AABBShapeComponent) return false;
    auto size = AABBShapeComponent->size;
    if(size.x < 0.1f || size.y < 0.1f) return false;
    return true;
}

bool Actor::isOnGround() {
    if (VersionUtils::checkAboveOrEqual(21, 0)) {
        // might be needed when entt cant be used for .21
        //static uintptr_t sig = Memory::findSig(std::string(GET_SIG("tryGetPrefix3")) + " " + GET_SIG("Actor::getOnGroundFlagComponent"));
        return hasComponent<OnGroundFlagComponent>();
    }
    const auto ctx = this->GetEntityContextV1_20_50();

    if (VersionUtils::checkAboveOrEqual(20, 60)) {
        using isOnGroundFunc = bool (__fastcall *)(entt::basic_registry<EntityId> &, const EntityId &);
        static isOnGroundFunc isOnGround = Memory::getOffsetFromSig<isOnGroundFunc>(
            GET_SIG_ADDRESS("ActorCollision::isOnGround"), 1);

        if (isOnGround)
            return isOnGround(ctx.enttRegistry, ctx.entity);

        return false;
    }

    using isOnGroundFunc = bool (__fastcall *)(const V1_20_50::EntityContext&);
    static isOnGroundFunc isOnGround = reinterpret_cast<isOnGroundFunc>(GET_SIG_ADDRESS("ActorCollision::isOnGround"));

    if (isOnGround)
        return isOnGround(ctx);

    return false;
}

Vec3<float> Actor::getLerpedPosition() {
    auto* renderPos = this->getRenderPositionComponent();
    if (!renderPos) return Vec3<float>{0.f, 0.f, 0.f};
    return renderPos->renderPos;
}

AABB Actor::getLerpedAABB(bool asHitbox) {
    auto* renderPosComp = this->getRenderPositionComponent();
    auto* aabbComp = this->getAABBShapeComponent();
    if (!renderPosComp || !aabbComp) return {};
    auto renderPos = renderPosComp->renderPos;
    auto aabbSize = aabbComp->size;

    float mod = 0.f;

    if (this->hasCategory(ActorCategory::Player))
        mod = 1.6f;

    auto lower = renderPos.sub(aabbSize.x / 2.f, mod, aabbSize.x / 2.f), upper = lower.add(aabbSize.x, aabbSize.y, aabbSize.x);

    auto aabb = AABB(lower, upper);

    if(asHitbox)
        return aabb.expandedXYZ(0.1);

    return aabb;
}

float Actor::getApproximateReach(Actor *target) {
    auto actorHitBox = target->getLerpedAABB(true);

    auto upper = actorHitBox.upper;
    auto lower = actorHitBox.lower;

    auto posAtTimeOfHit = this->getLerpedPosition();

    auto closestPoint = Vec3<float>{ std::clamp(posAtTimeOfHit.x, lower.x, upper.x),
                                     std::clamp(posAtTimeOfHit.y, lower.y, upper.y),
                                     std::clamp(posAtTimeOfHit.z, lower.z, upper.z) };

    return posAtTimeOfHit.dist(closestPoint);
}

bool Actor::IsOnSameTeam(Actor *actor) {
    std::string playerName = *this->getNametag();
    std::string actorName = *actor->getNametag();

    if (playerName.empty() || actorName.empty()) return false;

    auto cleanName = [](std::string &name) {
        constexpr std::string tags[] = {"§r", "§l"};
        for (const auto &tag : tags) {
            size_t pos;
            while ((pos = name.find(tag)) != std::string::npos) {
                name.erase(pos, tag.length());
            }
        }
    };

    cleanName(playerName);
    cleanName(actorName);

    size_t playerTeamPos = playerName.find("§");
    if (playerTeamPos == std::string::npos) return false;

    std::string playerTeam = playerName.substr(playerTeamPos + 2, 1);

    size_t actorTeamPos = actorName.find("§");
    if (actorTeamPos != std::string::npos) {
        std::string actorTeam = actorName.substr(actorTeamPos + 2, 1);
        return actorTeam == playerTeam;
    }

    return false;
}

AttributesComponent* Actor::getAttributesComponent() {
    static uintptr_t sig;

    if (!VersionUtils::checkAboveOrEqual(21, 00)) {
        return nullptr;
    }

    return tryGet<AttributesComponent>(sig);
}

float Actor::getHealth() {
    auto comp = getAttributesComponent();
    if (!comp) return 20.f;
    auto inst = comp->getInstance(static_cast<unsigned int>(GET_OFFSET("Attribute::Health")));
    if (!inst) return 20.f;
    return inst->getValue();
}

float Actor::getHunger() {
    auto comp = getAttributesComponent();
    if (!comp) return 20.f;
    auto inst = comp->getInstance(static_cast<unsigned int>(GET_OFFSET("Attribute::Hunger")));
    if (!inst) return 20.f;
    return inst->getValue();
}

float Actor::getSaturation() {
    auto comp = getAttributesComponent();
    if (!comp) return 20.f;
    auto inst = comp->getInstance(static_cast<unsigned int>(GET_OFFSET("Attribute::Saturation")));
    if (!inst) return 20.f;
    return inst->getValue();
}

int Actor::getPlayerLevel() {
    auto comp = getAttributesComponent();
    if (!comp) return 0;
    auto inst = comp->getInstance(static_cast<unsigned int>(GET_OFFSET("Attribute::PlayerLevel")));
    if (!inst) return 0;
    return static_cast<int>(inst->getValue());
}

float Actor::getExperienceProgress() {
    auto comp = getAttributesComponent();
    if (!comp) return 0.f;
    auto inst = comp->getInstance(static_cast<unsigned int>(GET_OFFSET("Attribute::PlayerExperience")));
    if (!inst) return 0.f;
    return inst->getValue();
}

MoveInputComponentHandler Actor::getHandler() {
    return MoveInputComponentHandler(this);
}

// MoveInputComponentHandler implementations

bool MoveInputComponentHandler::forward() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.forward() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.forward : false;
    }
}

bool MoveInputComponentHandler::backward() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.backward() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.backward : false;
    }
}

bool MoveInputComponentHandler::left() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.left() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.left : false;
    }
}

bool MoveInputComponentHandler::right() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.right() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.right : false;
    }
}

bool MoveInputComponentHandler::mSneakDown() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.mSneakDown() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.mSneakDown : false;
    }
}

bool MoveInputComponentHandler::mSprintDown() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.mSprintDown() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.mSprintDown : false;
    }
}

bool MoveInputComponentHandler::mJumpDown() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->mInputState.mJumpDown() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->mInputState.mJumpDown : false;
    }
}

bool MoveInputComponentHandler::getSneaking() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->getSneaking() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->sneaking : false;
    }
}

bool MoveInputComponentHandler::getSprinting() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->getSprinting() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->sprinting : false;
    }
}

bool MoveInputComponentHandler::getJumping() {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        return handler ? handler->getJumping() : false;
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        return handler ? handler->jumping : false;
    }
}

void MoveInputComponentHandler::setForward(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setForward(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.forward = value;
    }
}

void MoveInputComponentHandler::setBackward(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setBackward(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.backward = value;
    }
}

void MoveInputComponentHandler::setLeft(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setLeft(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.left = value;
    }
}

void MoveInputComponentHandler::setRight(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setRight(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.right = value;
    }
}

void MoveInputComponentHandler::setMSneakDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setMSneakDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.mSneakDown = value;
    }
}

void MoveInputComponentHandler::setMSprintDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setMSprintDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.mSprintDown = value;
    }
}

void MoveInputComponentHandler::setMJumpDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mInputState.setMJumpDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mInputState.mJumpDown = value;
    }
}

void MoveInputComponentHandler::setSneaking(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->setSneaking(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->sneaking = value;
    }
}

void MoveInputComponentHandler::setSprinting(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->setSprinting(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->sprinting = value;
    }
}

void MoveInputComponentHandler::setJumping(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->setJumping(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->jumping = value;
    }
}

void MoveInputComponentHandler::setRawForward(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setForward(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.forward = value;
    }
}

void MoveInputComponentHandler::setRawBackward(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setBackward(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.backward = value;
    }
}

void MoveInputComponentHandler::setRawLeft(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setLeft(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.left = value;
    }
}

void MoveInputComponentHandler::setRawRight(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setRight(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.right = value;
    }
}

void MoveInputComponentHandler::setRawMSneakDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setMSneakDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.mSneakDown = value;
    }
}

void MoveInputComponentHandler::setRawMSprintDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setMSprintDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.mSprintDown = value;
    }
}

void MoveInputComponentHandler::setRawMJumpDown(bool value) {
    if (VersionUtils::checkAboveOrEqual(21, 120)) {
        auto* handler = actor->getMoveInputHandler();
        if (handler) handler->mRawInputState.setMJumpDown(value);
    } else {
        auto* handler = actor->getMoveInputHandlerOLD();
        if (handler) handler->mRawInputState.mJumpDown = value;
    }
}

// Explicit template instantiations for components used outside Actor.cpp
template ActorUniqueIDComponent* Actor::tryGet<ActorUniqueIDComponent>(uintptr_t);
template ActorDefinitionIdentifierComponent* Actor::tryGet<ActorDefinitionIdentifierComponent>(uintptr_t);
template MobBodyRotationComponent* Actor::tryGet<MobBodyRotationComponent>(uintptr_t);