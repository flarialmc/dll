#pragma once

#include "../Utils.hpp"

class PositionUtils {
public:
    static Vec2<float> getUIScale();
    static Vec2<float> getScreenScale();

    static Vec2<float> getScaledPos(Vec2<float> pos);
    static Vec2<float> getScreenScaledPos(Vec2<float> pos);
    static Vec2<float> getCustomScreenScaledPos(Vec2<float> pos, float scale);
};
