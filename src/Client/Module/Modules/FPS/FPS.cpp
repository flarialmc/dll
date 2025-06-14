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
    setDef("fpsSpoofer", 1.0f);
    if (ModuleManager::initialized) Client::SaveSettings();
}

void FPS::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("FPS Counter");
    defaultAddSettings("main");
    addSlider("FPS Spoofer", "Adjusts the displayed FPS.", "fpsSpoofer", 10.0f);
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
        float fpsSpooferValue = getOps<float>("fpsSpoofer");

        if (fpsSpooferValue > 10.0f) {
            fpsSpooferValue = 10.0f;
        }
        if (fpsSpooferValue < 1.0f) {
            fpsSpooferValue = 1.0f;
        }

        getOps<float>("fpsSpoofer") = fpsSpooferValue;

        int fps = (int)round(((float)MC::fps * round(fpsSpooferValue)));

        auto fpsStr = FlarialGUI::cached_to_string(fps);

        this->normalRender(0, fpsStr);
    }
}
