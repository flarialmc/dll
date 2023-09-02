#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class FogColorHook : public Hook
{
private:

	static float* FogColorCallback(void* a1, void* a2, void* a3, void* a4) {


        auto* color = new MCCColor();

		if (ModuleManager::getModule("Fog Color") != nullptr ) {
            if (ModuleManager::getModule("Fog Color")->settings.getSettingByName<bool>("enabled")->value) {

                D2D1_COLOR_F color2;
                if (ModuleManager::getModule("Fog Color")->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
                else color2 = FlarialGUI::HexToColorF(ModuleManager::getModule("Fog Color")->settings.getSettingByName<std::string>("color")->value);

                color->r = color2.r;
                color->g = color2.g;
                color->b = color2.b;
                color->a = ModuleManager::getModule("Fog Color")->settings.getSettingByName<float>("colorOpacity")->value;
                return color->arr;

            }
        }

        return func_original(a1, a2, a3, a4);

    }

public:
    typedef float*(__thiscall* FogColorOriginal)(void*, void*, void*, void*);
    static inline FogColorOriginal func_original = nullptr;

    FogColorHook() : Hook("Fog Color Hook", "41 0F 10 08 48 8B C2") {}

    void enableHook() override {

        this->autoHook(FogColorCallback, (void**)&func_original);

    }
};


