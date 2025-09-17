#pragma once

#include "../../../../Utils/Utils.hpp"
#include "../EntityContext.hpp"

struct ActorRotationComponent : IEntityComponent {
    Vec2<float> rot;
    Vec2<float>  rotPrev;
};
static_assert(sizeof(ActorRotationComponent) == 0x10);