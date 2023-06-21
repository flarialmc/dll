#pragma once
#include "d2d1.h"
#include "../../../Utils/Utils.hpp"

class Constraints
{
public:
    static float PercentageConstraint(float percentage, const std::string &edge, bool ignore_stack = false);
    static float RelativeConstraint(float percent, const std::string&  = "height", bool ignore_stack = false);
    static Vec2<float> CenterConstraint(float width, float height, const std::string& axis = "no", float xmodifier = 0.f, float ymodifier = 0.f, bool ignore_stack = false);
    static Vec2<float> RoundingConstraint(float radiusX, float radiusY, bool ignore_stack = false);

    static float SpacingConstraint(float percentage, float dimension);

    static float FontScaler(float dimension);

    static Vec2<float> CalculatePercentage(float x, float y);
};