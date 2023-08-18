#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"
#include <cmath>

class getSensHook : public Hook
{
private:

	static inline float currSens = 0.0f;
	static inline bool saved = false;

	// max sens = 0.858693;

	static float getSensCallback(unsigned int* a1, unsigned int a2) {

		float sens = func_original(a1, a2);

		if (ModuleManager::getModule("Zoom") != nullptr) {
			auto zom = reinterpret_cast<Zoom*>(ModuleManager::getModule("Zoom"));
			if (zom->settings.getSettingByName<bool>("enabled")->value) {
				if (!saved) {
					saved = true;
					currSens = sens;
				}
				sens = currSens - (currSens * (((90-ZoomListener::zoomValue) / 90.0f) / 1.25f));
			}
			else if (saved) {
				saved = false;
			}
		}

		return sens;
	}

public:
	typedef float(__thiscall* getSensOriginal)(unsigned int*, unsigned int);
	static inline getSensOriginal func_original = nullptr;

	getSensHook() : Hook("getSensHook", "4C 8B D1 44 0F B6 CA 49 BB B3 01 00 00 00 01 00 00 48 B8 25 23 22 84 E4 9C F2 CB 4C 33 C8 8B C2 4D 0F AF CB C1 E8 08 44 0F B6 C0 8B C2 4D 33 C8 C1 E8 10 4D 8B 42 30") {}

	void enableHook() override {
		this->autoHook(getSensCallback, (void**)&func_original);
	}
};


