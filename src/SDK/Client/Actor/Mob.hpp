#pragma once

#include "Actor.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "Components/MobBodyRotationComponent.hpp"

class Mob : public Actor {
public:
    BUILD_ACCESS(this, MobHurtTimeComponent*, mobHurtTimeComponent, 0x608);
    void setSprinting(bool state);
    MobBodyRotationComponent* getBodyRotationComponent();
};