#pragma once
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"

class GuiScaleListener : public Listener {

    Module* module;
public:

    void onRender(RenderEvent& event) override {
        if(SDK::currentScreen == "hud_screen")
            if(SDK::clientInstance->guiData != nullptr)
        if (module->settings.getSettingByName<bool>("enabled")->value){
        float percent = module->settings.getSettingByName<float>("guiscale")->value;
        SDK::clientInstance->guiData->GuiScale = percent;
        SDK::clientInstance->guiData->ScreenSizeScaled = Vec2{ SDK::clientInstance->guiData->ScreenSize.x * 1 / percent, SDK::clientInstance->guiData->ScreenSize.y * 1 / percent };
        SDK::clientInstance->guiData->scalingMultiplier = 1 / percent;
        }
    }


public:
    explicit GuiScaleListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
        // reset shit on init
    }
};
