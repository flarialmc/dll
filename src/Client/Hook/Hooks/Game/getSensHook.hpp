#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"
#include <cmath>

double easeInOutCubic(float t) {
    return t < 0.5 ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2;
}

class getSensHook : public Hook
{
private:

    static inline float currentSens = 0.0f;

    // max sens = 0.858693;

    static float getSensCallback(int *a1, int a2) {

        float sens = func_original(a1, a2);

        //std::cout << sens << std::endl;


        //if (ModuleManager::getModule("Zoom") != nullptr && fov != 70) {
        //
        //    auto zom = reinterpret_cast<Zoom*>(ModuleManager::getModule("Zoom"));
        //    if (zom->settings.getSettingByName<bool>("enabled")->value) {
        //
        //    }
        //
        //}

        //return easeInOutCubic(sens);

        return sens;
    }

public:
    typedef float(__thiscall* getSensOriginal)(int*, int);
    static inline getSensOriginal func_original = nullptr;

    getSensHook() : Hook("getSensHook", "4C 8B D1 44 0F B6 CA 49 BB B3 01 00 00 00 01 00 00 48 B8 25 23 22 84 E4 9C F2 CB 4C 33 C8 8B C2 4D 0F AF CB C1 E8 08 44 0F B6 C0 8B C2 4D 33 C8 C1 E8 10 4D 8B 42 30") {}

    void enableHook() override {
        this->autoHook(getSensCallback, (void**)&func_original);
    }
};


