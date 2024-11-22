#pragma once
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"

class GuiScaleListener : public Listener {
private:
    float originalScale = 0.f;
    Module* module;
public:

    void onSetupAndRender(SetupAndRenderEvent& event) override {
        update();
    }

    void update() {
        float targetScale = module->settings.getSettingByName<float>("guiscale")->value;
        auto guiData = SDK::clientInstance->getGuiData();
        if(guiData->GuiScale == targetScale) return;

        updateScale(targetScale);
    }

    void updateScale(float newScale) {
        auto guiData = SDK::clientInstance->getGuiData();

        if(originalScale == 0) {
            originalScale = guiData->GuiScale;
        }
        float oldScale = guiData->GuiScale;

        auto screenSize = guiData->ScreenSize;
        static auto safeZone = Vec2<float>{ 0.f, 0.f };

        SDK::clientInstance->_updateScreenSizeVariables(&screenSize, &safeZone, newScale < 1.f ? 1.f : newScale);
    }


public:
    explicit GuiScaleListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
        // reset shit on init
    }
};
