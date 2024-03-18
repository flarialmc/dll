#include "MouseHook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"

MouseHook::MouseHook() : Hook("mouse_hook",
                              "48 8B C4 48 89 58 ?? 48 89 68 ?? 48 89 70 ?? 57 41 54 41 55 41 56 41 57 48 83 EC ?? 44 0F B7 BC 24") {
}

void MouseHook::enableHook() {
    this->autoHook((void *) mouseCallback, (void **) &funcOriginal);
}

void MouseHook::mouseCallback(void *mouseDevice, char button, char action, short mouseX, short mouseY,
                              short movementX,
                              short movementY, char a8) {

    // eventemitter here

    // BUTTON
    // 0 -> Mouse move, mouseX,mouseY
    // 4 -> Mouse wheel, state/WHEEL_DELTA
    // rest -> Mouse button, state

    // parm_1, parm_8 (might be isScrolling?) -> ???

    MouseEvent event(button, action, mouseX, mouseY);
    EventHandler::onMouse(event);

    if (!event.isCancelled()) {
        return funcOriginal(mouseDevice, event.getButton(), event.getActionAsChar(), mouseX, mouseY, movementX,
                            movementY, a8);
    }
}