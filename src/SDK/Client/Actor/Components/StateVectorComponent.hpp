#pragma once

#include "../../../../Utils/Utils.hpp"
#include "../EntityContext.hpp"

struct StateVectorComponent : IEntityComponent {
public:
    Vec3<float> Pos;
    Vec3<float> PrevPos;
    Vec3<float> velocity;
};
static_assert(sizeof(StateVectorComponent) == 0x24);