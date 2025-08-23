#include "HueChanger.hpp"

#include "Events/EventManager.hpp"

void HueChanger::onEnable() {
    Listen(this, RenderEvent, &HueChanger::onRender)
    Module::onEnable();
}

void HueChanger::onDisable() {
    Deafen(this, RenderEvent, &HueChanger::onRender)
    Module::onDisable();
}

void HueChanger::defaultConfig() {
    setDef("intensity", 0.5f);
    Module::defaultConfig("core");
    
}

void HueChanger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Saturation");
    addSlider("Saturation Intensity", "", "intensity", 3.0f);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HueChanger::onRender(RenderEvent &event) {
    if (this->isEnabled()) {
        FlarialGUI::ApplyHue(getOps<float>("intensity"));
    }
}
