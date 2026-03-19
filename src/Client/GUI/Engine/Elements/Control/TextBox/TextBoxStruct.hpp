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

    void reset() {
        text.clear();
        isActive = false;
        firstTime = true;
        isDeleting = false;
        isMovingLeft = false;
        isMovingRight = false;
        isForwardDeleting = false;
        cursorOpac = 1.0f;
        cursorX = 0.0f;
        isAt1 = false;
        noCursorBgCol = true;
        curBgCol = {};
        isHovering = false;
        scrollOffset = 0.0f;
    }
};