#pragma once

struct TextBoxStruct {
    std::string text;
    bool isActive = false;
    bool firstTime = true;
    bool isDeleting = false;
    bool isMovingLeft = false;
    bool isMovingRight = false;
    bool isForwardDeleting = false;
    float cursorOpac = 1.0f;
    float cursorX = 0.0f;
    bool isAt1 = false;
    bool noCursorBgCol = true;
    D2D_COLOR_F curBgCol;
    bool isHovering = false;
    float scrollOffset = 0.0f;
};