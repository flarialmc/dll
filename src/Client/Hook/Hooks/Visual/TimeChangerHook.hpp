#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class TimeChangerHook : public Hook
{
private:

	static float TimeChangerCallback(void* a1, void* a2, void* a3) {

        float time = func_original(a1, a2, a3);

		if (ModuleManager::getModule("Time Changer") != nullptr ) {

            if (ModuleManager::getModule("Time Changer")->settings.getSettingByName<bool>("enabled")->value) {

                time = ModuleManager::getModule("Time Changer")->settings.getSettingByName<float>("time")->value;

            }
        }

        return time;

	}

public:
    typedef float(__thiscall* TimeChangerOriginal)(void* a1, void* color, void* a3);
    static inline TimeChangerOriginal func_original = nullptr;

    TimeChangerHook() : Hook("Time Changer Hook", "44 8B C2 B8 F1 19 76 05 F7 EA") {}

    void enableHook() override {

        this->autoHook(TimeChangerCallback, (void**)&func_original);
    }
};


