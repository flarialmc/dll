#pragma once

#include "../../../../../../Utils/Utils.hpp"

struct ColorPicker {
    std::string oldHex;
    float oldOpac{};
    std::string newHex;
    bool isActive = false;
    float hueX = 0.0f;
    float oldHueX = 0.0f;
    bool movingHueX = false;
    float opacX = 0.0f;
    bool movingOpacX = false;
    Vec2<float> shade = Vec2<float>{-1, -1};
    bool movingShade = false;
};
