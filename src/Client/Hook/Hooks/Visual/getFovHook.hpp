#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class getFovHook : public Hook
{
private:

    static inline float currentZoomVal = 0.0f;

	static float getFovCallback(void* a1, float f, void* a3, void* a4) {
		float fov = func_original(a1, f, a3, a4);

        if(fov > 100.0) fov -= 10.0f;

		if (ModuleManager::getModule("Zoom") != nullptr && fov != 70.00f) {

            auto zom = reinterpret_cast<Zoom *>(ModuleManager::getModule("Zoom"));
            if(zom->settings.getSettingByName<bool>("enabled")->value) currentZoomVal = std::lerp(currentZoomVal, ZoomListener::zoomValue, 0.05f);
            else currentZoomVal = std::lerp(currentZoomVal, fov, 0.05f);
            return currentZoomVal;

        } else return fov;


	}

public:
    typedef float(__thiscall* getFovOriginal)(void*, float, void*, void*);
    static inline getFovOriginal func_original = nullptr;

    getFovHook() : Hook("getFovHook", "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F") {}

    void enableHook() override {
        this->autoHook(getFovCallback, (void**)&func_original);
    }
};


