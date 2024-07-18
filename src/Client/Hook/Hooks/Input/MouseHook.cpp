#include "MouseHook.hpp"

#include <imgui.h>

#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

MouseHook::MouseHook() : Hook("mouse_hook",GET_SIG("MouseDevice::feed")) {
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


    ImGui::GetIO().AddMousePosEvent(mouseX, mouseY);
    ImGui::GetIO().AddMouseButtonEvent(button - 1, action);

    if (!event.isCancelled()) {
        return funcOriginal(mouseDevice, event.getButton(), event.getActionAsChar(), mouseX, mouseY, movementX,
                            movementY, a8);
    }
}