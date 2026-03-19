#pragma once

struct RangeSliderRect {
    // Left handle state (minimum value)
    bool isMovingLeftHandle = false;
    bool leftHasBeenMoved = false;
    float leftMovedX = 0;
    float leftPercentageX = 0;
    float leftHoveredAnim = 0;
    bool leftFirstHover = false;
    bool leftIsValid = true;
    float leftValidFloat = -1.f;

    // Right handle state (maximum value)
    bool isMovingRightHandle = false;
    bool rightHasBeenMoved = false;
    float rightMovedX = 0;
    float rightPercentageX = 0;
    float rightHoveredAnim = 0;
    bool rightFirstHover = false;
    bool rightIsValid = true;
    float rightValidFloat = -1.f;
};
