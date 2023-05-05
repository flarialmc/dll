#include "MouseHook.h"

#include "../../../FlarialGUI/FlarialWidgets.h"

typedef void(__fastcall* onMouse)(void*, MouseAction, int8_t, short, short, void*, void*, void*);
onMouse oMouse;

void MouseDetour(void* a1, MouseAction action, int8_t held, int16_t mouseX, int16_t mouseY, void* a6, void* a7, void* a8) {
	bool cancel = false;

	if (ImGui::GetCurrentContext() != nullptr) {
		ImGuiIO& io = ImGui::GetIO();

		io.AddMousePosEvent((float)mouseX, (float)mouseY);

		if (ImGui::GetCurrentContext() && action < MouseAction::Scroll && action != MouseAction::None) {
			io.MouseDown[action - 1] = held;
		}
		else if (action == MouseAction::Scroll) {
			io.MouseWheel = held < 0 ? -0.5f : 0.5f;
		}
	}
	MC::mousepos.x = mouseX;
	MC::mousepos.y = mouseY;

	if(action == Scroll)
	{
		FlarialGUI::scrollpos += held < 0 ? 0 - FlarialGUI::scrollposmodifier : FlarialGUI::scrollposmodifier;
		FlarialGUI::barscrollpos += held < 0 ? 0 - FlarialGUI::barscrollposmodifier : FlarialGUI::barscrollposmodifier;
		Logger::debug(std::to_string(FlarialGUI::scrollpos));
	}
	
	MC::mouseaction = action;
	MC::held = held;
	
	Manager::onMouse(action, cancel);
	if (!cancel) {
		return oMouse(a1, action, held, mouseX, mouseY, a6, a7, a8);
	}
}

MouseHook::MouseHook() : Hook() {
	addr = Mem::findSig(Signatures::Mouse);

	Mem::hookFunc((void*)addr, (void*)MouseDetour, (void**)&oMouse, "Mouse");
}