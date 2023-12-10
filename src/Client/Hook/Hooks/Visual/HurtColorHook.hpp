#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class HurtColorHook : public Hook
{
private:

	static MCCColor* HurtColorCallback(void* a1, MCCColor* color, void* a3) {



		if (ModuleManager::getModule("Hurt Color") != nullptr ) {

            if(ModuleManager::getModule("Hurt Color")->settings.getSettingByName<bool>("enabled")->value) {

                D2D1_COLOR_F color2;
                if (ModuleManager::getModule("Hurt Color")->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
                else color2 = FlarialGUI::HexToColorF(ModuleManager::getModule("Hurt Color")->settings.getSettingByName<std::string>("color")->value);

                color->r = color2.r;
                color->g = color2.g;
                color->b = color2.b;
                color->a = ModuleManager::getModule("Hurt Color")->settings.getSettingByName<float>(
                        "colorOpacity")->value;
                return color;
            }

        }

        return func_original(a1, color, a3);

    }

public:
    typedef MCCColor*(__thiscall* HurtColorOriginal)(void* a1, MCCColor* color, void* a3);
    static inline HurtColorOriginal func_original = nullptr;

    HurtColorHook() : Hook("Hurt Color Hook", "") {}

    void enableHook() override {

        auto RefAddr = Memory::findSig("E8 ? ? ? ? E9 ? ? ? ? 8B 43 18 89 44 24 ? 48 8D 54 24 ? 48 8B 4B ? E8 ? ? ? ? 48 8B F8");
        auto RealFunc = RefAddr + 1 + 4 + *reinterpret_cast<int*>(RefAddr + 1);


        this->manualHook((void*) RealFunc, HurtColorCallback, (void**)&func_original);
    }
};


