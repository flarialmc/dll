#pragma once

#include "Actor.hpp"
#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class Mob : public Actor {
public:
    BUILD_ACCESS(this, MobHurtTimeComponent*, mobHurtTimeComponent, 0x660);
    void setSprinting(bool state);
};