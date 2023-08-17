#pragma once
#include "../../../../Utils/Utils.hpp"

class StateVectorComponent
{
public:
    BUILD_ACCESS(this, Vec3<float>, pos, 0x0);
    BUILD_ACCESS(this, Vec3<float>, posDelta, 0x18);
    BUILD_ACCESS(this, Vec3<float>, posPrev, 0x0C);
    Vec3<float> Pos;
    Vec3<float> PrevPos;
    Vec3<float> velocity;
};