#pragma once

#include <cstdint>
#include <string>
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "Components/MoveInputComponent.hpp"
#include "Components/ActorMovementPorxyComponent.hpp"
#include "Components/ActorRotationComponent.hpp"
#include "Components/MobHurtTimeComponent.hpp"
#include "../Level/Level.h"
#include "../../../Utils/Utils.hpp"

enum ActorFlags
{
    FLAG_ONFIRE = 0,
    FLAG_SNEAKING = 1,
    FLAG_RIDING = 2,
    FLAG_SPRINTING = 3,
    FLAG_ACTION = 4,
    FLAG_INVISIBLE = 5,
    FLAG_TEMPTED = 6,
    FLAG_INLOVE = 7,
    FLAG_SADDLED = 8,
    FLAG_POWERED = 9,
    FLAG_IGNITED = 10,
    FLAG_BABY = 11,
    FLAG_CONVERTING = 12,
    FLAG_CRITICAL = 13,
    FLAG_CAN_SHOW_NAMETAG = 14,
    FLAG_ALWAYS_SHOW_NAMETAG = 15,
    FLAG_IMMOBILE = 16,
    FLAG_NO_AI = FLAG_IMMOBILE,
    FLAG_SILENT = 17,
    FLAG_WALLCLIMBING = 18,
    FLAG_CAN_CLIMB = 19,
    FLAG_SWIMMER = 20,
    FLAG_CAN_FLY = 21,
    FLAG_WALKER = 22,
    FLAG_RESTING = 23,
    FLAG_SITTING = 24,
    FLAG_ANGRY = 25,
    FLAG_INTERESTED = 26,
    FLAG_CHARGED = 27,
    FLAG_TAMED = 28,
    FLAG_ORPHANED = 29,
    FLAG_LEASHED = 30,
    FLAG_SHEARED = 31,
    FLAG_GLIDING = 32,
    FLAG_ELDER = 33,
    FLAG_MOVING = 34,
    FLAG_BREATHING = 35,
    FLAG_CHESTED = 36,
    FLAG_STACKABLE = 37,
    FLAG_SHOWBASE = 38,
    FLAG_REARING = 39,
    FLAG_VIBRATING = 40,
    FLAG_IDLING = 41,
    FLAG_EVOKER_SPELL = 42,
    FLAG_CHARGE_ATTACK = 43,
    FLAG_WASD_CONTROLLED = 44,
    FLAG_CAN_POWER_JUMP = 45,
    FLAG_CAN_DASH = 46,
    FLAG_LINGER = 47,
    FLAG_HAS_COLLISION = 48,
    FLAG_GRAVITY = 49,
    FLAG_FIRE_IMMUNE = 50,
    FLAG_DANCING = 51,
    FLAG_ENCHANTED = 52,
    FLAG_SHOW_TRIDENT_ROPE = 53,
    FLAG_CONTAINER_PRIVATE = 54,
    FLAG_IS_TRANSFORMING = 55,
    FLAG_SPIN_ATTACK = 56,
    FLAG_SWIMMING = 57,
    FLAG_BRIBED = 58,
    FLAG_PREGNANT = 59,
    FLAG_LAYING_EGG = 60,
    FLAG_RIDER_CAN_PICK = 61,
    FLAG_TRANSITION_SITTING = 62,
    FLAG_EATING = 63,
    FLAG_LAYING_DOWN = 64,
    FLAG_SNEEZING = 65,
    FLAG_TRUSTING = 66,
    FLAG_ROLLING = 67,
    FLAG_SCARED = 68,
    FLAG_IN_SCAFFOLDING = 69,
    FLAG_OVER_SCAFFOLDING = 70,
    FLAG_FALL_THROUGH_SCAFFOLDING = 71,
    FLAG_BLOCKING = 72,
    FLAG_TRANSITION_BLOCKING = 73,
    FLAG_BLOCKED_USING_SHIELD = 74,
    FLAG_BLOCKED_USING_DAMAGED_SHIELD = 75,
    FLAG_SLEEPING = 76,
    FLAG_ENTITY_GROW_UP = 77,
    FLAG_TRADE_INTEREST = 78,
    FLAG_DOOR_BREAKER = 79,
    FLAG_BREAKING_OBSTRUCTION = 80,
    FLAG_DOOR_OPENER = 81,
    FLAG_IS_ILLAGER_CAPTAIN = 82,
    FLAG_STUNNED = 83,
    FLAG_ROARING = 84,
    FLAG_DELAYED_ATTACK = 85,
    FLAG_IS_AVOIDING_MOBS = 86,
    FLAG_IS_AVOIDING_BLOCKS = 87,
    FLAG_FACING_TARGET_TO_RANGE_ATTACK = 88,
    FLAG_HIDDEN_WHEN_INVISIBLE = 89,
    FLAG_IS_IN_UI = 90,
    FLAG_STALKING = 91,
    FLAG_EMOTING = 92,
    FLAG_CELEBRATING = 93,
    FLAG_ADMIRING = 94,
    FLAG_CELEBRATING_SPECIAL = 95,
    FLAG_RAM_ATTACK = 97,
    FLAG_PLAYING_DEAD = 98,
    FLAG_IN_ASCENDABLE_BLOCK = 99,
    FLAG_OVER_DESCENDABLE_BLOCK = 100,
    FLAG_CROAKING = 101,
    FLAG_EAT_MOB = 102,
    FLAG_JUMP_GOAL_JUMP = 103,
    FLAG_EMERGING = 104,
    FLAG_SNIFFING = 105,
    FLAG_DIGGING = 106,
    FLAG_SONIC_BOOM = 107,
    FLAG_HAS_DASH_COOLDOWN = 108,
    FLAG_PUSH_TOWARDS_CLOSEST_SPACE = 109,
    FLAG_SCENTING = 110,
    FLAG_RISING = 111,
    FLAG_FEELING_HAPPY = 112,
    FLAG_SEARCHING = 113
};

class Actor {
public:

    BUILD_ACCESS(this, Level*, level, 0x260);
    BUILD_ACCESS(this, ActorRotationComponent*, actorRotationComponent, 0x2B0);


    template <typename Component>
    Component* tryGet(uintptr_t addr);
  
    ActorMovementProxyComponent* getMovementProxyComponent();
    MoveInputComponent* getMoveInputHandler();
    bool getActorFlag(int flag);
    Vec3<float>* getPosition();
    bool wasHurt();
};
