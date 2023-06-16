#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Manager.hpp"

class getFovHook : public Hook
{
private:
	static float getFovCallback(void* a1, float f, void* a3, void* a4) {
		float fov = func_original(a1, f, a3, a4);

		constexpr float zoomValue = 0.75f;
		static float animValue = 0.0f;

		if (ModuleManager::getModule("Zoom") != nullptr) {
			auto zom = reinterpret_cast<Zoom*>(ModuleManager::getModule("Zoom"));
			if (zom->enabled) {
				animValue = (std::min)(animValue + (f * 0.1f), 1.0f);
			}
			else {
				animValue = (std::max)(animValue - (f * 0.1f), 0.0f);
			}
		}
		
		fov -= (animValue * animValue * zoomValue * fov);
		return fov;
	}

public:
    typedef float(__fastcall* getFovOriginal)(void*, float, void*, void*);
    static inline getFovOriginal func_original;

    getFovHook() : Hook("getFovHook", "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F") {}
    
	void enableHook() override {
		this->autoHook(getFovCallback, (void**)&func_original);
	}
};


