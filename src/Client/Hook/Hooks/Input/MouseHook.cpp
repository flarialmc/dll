#include "MouseHook.hpp"
#include "KeyHook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Client.hpp"

MouseHook::MouseHook() : Hook("mouse_hook", "48 8B C4 48 89 58 ?? 48 89 68 ?? 48 89 70 ?? 57 41 54 41 55 41 56 41 57 48 83 EC ?? 44 0F B7 BC 24")
{
}

void MouseHook::enableHook()
{
	this->autoHook(mouseCallback, (void**)&func_original);
}

void MouseHook::mouseCallback(void* MouseDevice, char button, char action, short mouse_x, short mouse_y, short relativeMouseX,
	short relativeMouseY, bool isScrolling)
{

	// eventemitter here

	// BUTTON
	// 0 -> Mouse move, mouse_x,mouse_y
	// 4 -> Mouse wheel, state/WHEEL_DELTA
	// rest -> Mouse button, state

	// parm_1,parm_8 -> ???

	MouseEvent event(button, action, mouse_x, mouse_y);
	EventHandler::onMouse(event);

	if (!event.isCancelled()) {
		
		return func_original(MouseDevice, event.GetButton(), event.GetActionAsChar(), mouse_x, mouse_y, relativeMouseX, relativeMouseY, isScrolling);
	}
}