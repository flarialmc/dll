#include "FPS.hpp"

std::string FPS::getDisplayValue() {
    float fps = MC::fps;

    if (getOps<bool>("spoof")) {
        float fpsSpooferValue = getOps<float>("fpsSpoofer");

        if (fpsSpooferValue > 10.0f) {
            fpsSpooferValue = 10.0f;
        }
        if (fpsSpooferValue < 1.0f) {
            fpsSpooferValue = 1.0f;
        }

        getOps<float>("fpsSpoofer") = fpsSpooferValue;

        fps *= round(fpsSpooferValue);
    }

    return FlarialGUI::cached_to_string((int)round(fps));
}

void FPS::customConfig() {
    setDef("text", (std::string)"FPS: {value}");
    setDef("spoof", false);
    setDef("fpsSpoofer", 1.0f);
}

void FPS::customSettings() {
    addToggle("Spoof FPS", "", "spoof");
    addConditionalSlider(getOps<bool>("spoof"), "Spoofer", "Adjusts the displayed FPS.", "fpsSpoofer", 10.0f);
}

void FPS::customInit() {
    Listen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
}

void FPS::customCleanup() {
    Deafen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
}

void FPS::onRenderUnderUI(RenderUnderUIEvent &event) {
    //Blur::RenderBlur(event.RTV, 3, 5.f);
}
