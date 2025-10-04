#include "MouseHook.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

MouseHook::MouseHook() : Hook("mouse_hook",GET_SIG_ADDRESS("MouseDevice::feed")) {
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
    auto event = nes::make_holder<MouseEvent>(button, action, mouseX, mouseY, movementX, movementY);
    eventMgr.trigger(event);

    if (!event->isCancelled()) {
        return funcOriginal(mouseDevice, event->getButton(), event->getActionAsChar(), mouseX, mouseY, movementX,
                            movementY, a8);
    }
}