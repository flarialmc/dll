#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"

class TimeChangerHook : public Hook {
private:

    static float TimeChangerCallback(void *a1, void *a2, void *a3) {

        float time = func_original(a1, a2, a3);

        // TODO: USE EVENTS

        if (ModuleManager::getModule("Time Changer") != nullptr) {

            if (ModuleManager::getModule("Time Changer")->isEnabled()) {

                time = ModuleManager::getModule("Time Changer")->settings.getSettingByName<float>("time")->value;

            }
        }

        return time;

    }

public:
    typedef float(__thiscall *TimeChangerOriginal)(void *a1, void *color, void *a3);

    static inline TimeChangerOriginal func_original = nullptr;

    TimeChangerHook() : Hook("Time Changer Hook", GET_SIG("TimeChanger")) {}

    void enableHook() override {

        this->autoHook((void *) TimeChangerCallback, (void **) &func_original);
    }
};


