#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class DimensionFogColorHook : public Hook
{
private:

    static MCCColor& DimensionFogColorCallback(Dimension* _this, MCCColor& result, MCCColor const& baseColor, float brightness) {


        auto color = MCCColor();

        if (ModuleManager::getModule("Fog Color") != nullptr) {
            if (ModuleManager::getModule("Fog Color")->settings.getSettingByName<bool>("enabled")->value) {

                D2D1_COLOR_F color2;
                if (ModuleManager::getModule("Fog Color")->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
                else color2 = FlarialGUI::HexToColorF(ModuleManager::getModule("Fog Color")->settings.getSettingByName<std::string>("color")->value);

                color.r = color2.r;
                color.g = color2.g;
                color.b = color2.b;
                color.a = ModuleManager::getModule("Fog Color")->settings.getSettingByName<float>("colorOpacity")->value;
                return color;

            }
        }

        return func_original(_this, result, baseColor, brightness);

    }

public:
    typedef MCCColor& (__thiscall* DimensionFogColorOriginal)(Dimension*, MCCColor&, MCCColor const&, float);
    static inline DimensionFogColorOriginal func_original = nullptr;

    DimensionFogColorHook() : Hook("Fog Color Hook", "41 0F 10 00 48 8B C2 0F 11 02") {}

    void enableHook() override {

        this->autoHook(DimensionFogColorCallback, (void**)&func_original);

    }
};


