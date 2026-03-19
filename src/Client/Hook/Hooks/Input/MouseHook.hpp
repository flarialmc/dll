#pragma once

#include "../Hook.hpp"

class MouseHook : public Hook {
private:
    static void mouseCallback(void *mouseDevice, char button, char state, short mouseX, short mouseY, short movementX,
                              short movementY, char a8);

    static void* InputHandler_tick(void*_this, void* a2, void* a3, void* a4);

public:
    typedef void(__thiscall *mouseOriginal)(void *mouseDevice, char mouseButton, char isDown, short mouseX, short mouseY,
                                            short relativeMovementX, short relativeMovementY, char a8);

    static inline mouseOriginal funcOriginal = nullptr;

    typedef void*(__thiscall *InputHandler_tickOriginal)(void *_this, void* a2, void* a3, void* a4);

    static inline InputHandler_tickOriginal func0 = nullptr;

    MouseHook();

    void enableHook() override;
};