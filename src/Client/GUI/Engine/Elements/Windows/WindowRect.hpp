#pragma once

struct WindowRect {
    bool isMovingElement = false;
    bool hasBeenMoved = false;
    float movedX = 0;
    float movedY = 0;
    bool madeRect = false;
    float percentageX = 0;
    float percentageY = 0;
    float ratio = 0;
    float oriMouseX = -1;
    float oriMouseY = -1;
    float fixer = 0;
    float width = 0;
    float height = 0;
};
