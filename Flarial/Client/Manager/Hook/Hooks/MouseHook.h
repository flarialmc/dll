#pragma once
#include "../Hook.h"
#include "../../Module/Module.h"

class MouseHook : public Hook {
public:
	MouseHook();
	/* MouseHook(Manager* mgr) : Hook<void, void*, MouseAction, bool, short, short, void*, void*, void*>(mgr, "MouseHook", Mem::findSig("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 54 41 55 41 56 41 57 48 83 EC 60"),
		[&](void* p1, MouseAction action, bool isDown, short p4, short p5, void* p6, void* p7, void* p8){
			
			auto _this = this->manager->getHook<void, void*, MouseAction, bool, short, short, void*, void*, void*>("MouseHook");
			
			if(_this) {

				bool cancel = false;

				auto mousePos = Vec2<short>(p4, p5);

				if (ImGui::GetCurrentContext() && action != MouseAction::NONE)
					ImGui::GetIO().MouseDown[0] = isDown;

				for (auto [type, category] : this->manager->categories) {

					for (auto mod : category->modules) {

						if (mod->isEnabled)
							mod->callEvent<MouseEvent>(MouseEvent{ mousePos, action, isDown, &cancel });

					};

				};

				if (!cancel)
					_this->_Func(p1, action, isDown, p4, p5, p6, p7, p8);

			};
			
		}
	) {}; */
};