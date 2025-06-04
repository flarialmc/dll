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
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

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
