#pragma once

#include "../../../../Utils/Utils.hpp"

class StateVectorComponent {
public:
    Vec3<float> Pos;
    Vec3<float> PrevPos;
    Vec3<float> velocity;
};