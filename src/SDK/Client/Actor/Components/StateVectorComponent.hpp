#pragma once

#include "../../../../Utils/Utils.hpp"

class StateVectorComponent {
public:
    BUILD_ACCESS(this, Vec3<float>, pos, GET_OFFSET("StateVectorComponent::pos"));
    BUILD_ACCESS(this, Vec3<float>, posDelta, GET_OFFSET("StateVectorComponent::posDelta"));
    BUILD_ACCESS(this, Vec3<float>, posPrev, GET_OFFSET("StateVectorComponent::posPrev"));
    Vec3<float> Pos;
    Vec3<float> PrevPos;
    Vec3<float> velocity;
};