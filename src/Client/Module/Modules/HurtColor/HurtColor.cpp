#include "HurtColor.hpp"

#include "Events/EventManager.hpp"

void HurtColor::onEnable() {
    Listen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
    Module::onEnable();
}

void HurtColor::onDisable() {
    Deafen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
    Module::onDisable();
}

void HurtColor::defaultConfig() {
    Module::defaultConfig("core");
    setDef("hurt", (std::string)"FFFFFF", 0.65f, false);
    if (ModuleManager::initialized) Client::SaveSettings();
}

void HurtColor::settingsRender(float settingsOffset) {

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Hurt Color");
    addColorPicker("Color", "", "hurt");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HurtColor::onGetHurtColor(HurtColorEvent &event) {
    if (!this->isEnabled()) return;
    D2D1_COLOR_F color = getColor("hurt");

    event.setHurtColorFromD2DColor(color, getOps<float>("colorOpacity"));
}
