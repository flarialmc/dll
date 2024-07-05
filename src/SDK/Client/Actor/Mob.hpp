#pragma once

#include "Actor.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "Components/MobBodyRotationComponent.hpp"

class Mob : public Actor {
public:
    BUILD_ACCESS(this, MobHurtTimeComponent*, mobHurtTimeComponent, GET_OFFSET("Gamemode::mobHurtTimeComponent"));

    void setSprinting(bool state);

    template<typename Component>
    Component *tryGetComponent(uintptr_t addr);

    MobBodyRotationComponent *getBodyRotationComponent();
};