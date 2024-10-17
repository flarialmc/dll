#pragma once

#include "../../../Utils/Utils.hpp"

namespace mce {
    class MathUtility {
    public:
        static Vec2<float> getRotationFromNegZToVector(const Vec3<float>& vec);
    };
}
