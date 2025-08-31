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
    settings.renameSetting("color", "colorOpacity", "color_rgb", "hurt");
    Module::defaultConfig("core");
    setDef("hurt", (std::string)"FFFFFF", 0.65f, false);
    
}

void HurtColor::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Hurt Color");
    addColorPicker("Color", "", "hurt");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void HurtColor::onGetHurtColor(HurtColorEvent &event) {
    if (!this->isEnabled()) return;
    D2D1_COLOR_F color = getColor("hurt");

    event.setHurtColorFromD2DColor(color, getOps<float>("hurtOpacity"));
}
