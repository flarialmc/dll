#pragma once

#include "../Hook.hpp"

class MouseHook : public Hook {
private:
    static void mouseCallback(void *mouseDevice, char button, char state, short mouseX, short mouseY, short movementX,
                              short movementY, char a8);

public:
    typedef void(__thiscall *mouseOriginal)(void *mouseDevice, char mouseButton, char isDown, short mouseX, short mouseY,
                                            short relativeMovementX, short relativeMovementY, char a8);

    static inline mouseOriginal funcOriginal = nullptr;

    MouseHook();

    void enableHook() override;
};