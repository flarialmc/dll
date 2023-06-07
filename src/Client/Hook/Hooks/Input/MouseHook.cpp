#include "MouseHook.hpp"

MouseHook::MouseHook() : Hook("mouse_hook", "48 8B C4 48 89 58 ?? 48 89 68 ?? 48 89 70 ?? 57 41 54 41 55 41 56 41 57 48 83 EC ?? 44 0F B7 BC 24")
{
}

void MouseHook::enableHook()
{
    this->autoHook(mouseCallback, (void **)&func_original);
}

void MouseHook::mouseCallback(void *parm_1, char button, char state, short mouse_x, short mouse_y, short parm_6,
                              short parm_7, char parm_8)
{
    bool block = false;

    // eventemitter here

    // BUTTON
    // 0 -> Mouse move, mouse_x,mouse_y
    // 4 -> Mouse wheel, state/WHEEL_DELTA
    // rest -> Mouse button, state

    // parm_1,parm_8 -> ???

    if (!block)
        return func_original(parm_1, button, state, mouse_x, mouse_y, parm_6, parm_7, parm_8);
}