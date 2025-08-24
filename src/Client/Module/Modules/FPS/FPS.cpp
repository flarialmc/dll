#include "FPS.hpp"

#include "Events/EventManager.hpp"


void FPS::onEnable() {
    Listen(this, RenderEvent, &FPS::onRender)
    Listen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
    Module::onEnable();
}

void FPS::onDisable() {
    Deafen(this, RenderEvent, &FPS::onRender)
    Deafen(this, RenderUnderUIEvent, &FPS::onRenderUnderUI)
    Module::onDisable();
}

void FPS::defaultConfig() {
    setDef("text", (std::string)"FPS: {value}");
    Module::defaultConfig("all");
    setDef("spoof", false);
    setDef("fpsSpoofer", 1.0f);
    
}

void FPS::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("FPS Counter");
    addToggle("Spoof FPS", "", "spoof");
    addConditionalSlider(getOps<bool>("spoof"), "Spoofer", "Adjusts the displayed FPS.", "fpsSpoofer", 10.0f);
    extraPadding();
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void FPS::onRenderUnderUI(RenderUnderUIEvent &event) {
    //Blur::RenderBlur(event.RTV, 3, 5.f);
}

void FPS::onRender(RenderEvent &event) {
    if (this->isEnabled()) {

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

        std::string fpsStr = FlarialGUI::cached_to_string((int)round(fps));

        this->normalRender(0, fpsStr);
    }
}
