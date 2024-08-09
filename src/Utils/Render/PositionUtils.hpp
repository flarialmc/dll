#pragma once

#include "../Utils.hpp"

class PositionUtils {
public:
    static Vec2<float> getUIScale();

    static Vec2<float> getScaledPos(Vec2<float> pos);
    static Vec2<float> getCustomScaledPos(Vec2<float> pos, float scale);
};
