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

	getSensHook() : Hook("getSensHook", "48 83 EC ? 48 8B 01 48 8D 54 24 ? 41 B8 17 01 00 00") {}

	void enableHook() override {
		this->autoHook(getSensCallback, (void**)&func_original);
	}
};


