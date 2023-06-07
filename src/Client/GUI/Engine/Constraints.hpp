#include "d2d1.h"
#include "../../../Utils/Utils.hpp"

class Constraints
{
public:
    static float PercentageConstraint(float percentage, const std::string &edge, bool ignore_stack = false);
    static float RelativeConstraint(float percent, std::string dimension, bool ignore_stack = false);
    static Vec2<float> CenterConstraint(float width, float height, std::string axis, float xmodifier, float ymodifier, bool ignore_stack = false);
    static Vec2<float> RoundingConstraint(float radiusX, float radiusY, bool ignore_stack = false);
};