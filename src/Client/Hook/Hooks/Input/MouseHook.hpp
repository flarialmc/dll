#pragma once

#include "../Hook.hpp"

class MouseHook : public Hook
{
private:
    static void mouseCallback(void *parm_1, char button, char state, short mouse_x, short mouse_y, short movement_x,
                              short movement_y, bool isScrolling);

public:
    typedef void(__thiscall *MouseOriginal)(void *a1, char mouseButton, char isDown, short mouseX, short mouseY,
                                            short relativeMovementX, short relativeMovementY, char a8);
    static inline MouseOriginal func_original = 0;

    MouseHook();
    void enableHook() override;
};