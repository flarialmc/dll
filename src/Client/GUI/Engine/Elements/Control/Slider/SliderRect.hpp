#pragma once

struct SliderRect {
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    float percentageX = 0;
    float hoveredAnim = 0;
};
