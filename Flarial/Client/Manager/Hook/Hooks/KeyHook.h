#pragma once
#include "../Hook.h"
#include "../../Module/Module.h"

class KeyHook : public Hook {
public:
	KeyHook();
	/* HookKey(Manager* mgr) : Hook<void, uint64_t, bool>(mgr, "KeyHook", Mem::findSig("48 ? ? 48 ? ? ? 4C 8D 05 ? ? ? ? 89"),
		[&](uint64_t key, bool isDown){
			
			bool cancel = false;
			auto _this = this->manager->getHook<void, uint64_t, bool>("KeyHook");
			
			if(_this) {

				this->manager->keyMap[key] = isDown;
				
				ClientInstance* instance = MC::getClientInstance();
				std::string currScreen = instance->getTopScreenName();

				bool canToggle = (isDown && currScreen.rfind("hud_screen") != std::string::npos);

				for (auto [type, category] : this->manager->categories) {

					for (auto mod : category->modules) {

						if (canToggle && mod->keybind == key)
							mod->isEnabled = !mod->isEnabled;

						if (mod->isEnabled)
							mod->callEvent<KeyEvent>(KeyEvent{ key, isDown, &cancel });

					};

				};

				if(!cancel)
					_this->_Func(key, isDown);

			};
			
		}
	) {}; */
};