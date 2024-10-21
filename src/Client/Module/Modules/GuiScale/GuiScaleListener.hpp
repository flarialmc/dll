#pragma once
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"

class GuiScaleListener : public Listener {

    Module* module;
public:

    void onRender(RenderEvent& event) override {
        if(SDK::getCurrentScreen() == "hud_screen")
            if(SDK::clientInstance->getGuiData() != nullptr)
        if (module->settings.getSettingByName<bool>("enabled")->value){
        float percent = module->settings.getSettingByName<float>("guiscale")->value;
        SDK::clientInstance->getGuiData()->GuiScale = percent;
        SDK::clientInstance->getGuiData()->ScreenSizeScaled = Vec2{ SDK::clientInstance->getGuiData()->ScreenSize.x * 1 / percent, SDK::clientInstance->getGuiData()->ScreenSize.y * 1 / percent };
        SDK::clientInstance->getGuiData()->scalingMultiplier = 1 / percent;
        }
    }


public:
    explicit GuiScaleListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
        // reset shit on init
    }
};
