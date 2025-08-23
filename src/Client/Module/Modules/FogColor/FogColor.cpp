#include "FogColor.hpp"

#include "Events/EventManager.hpp"

void FogColor::onEnable() {
    Listen(this, FogColorEvent, &FogColor::onGetFogColor)
    Module::onEnable();
}

void FogColor::onDisable() {
    Deafen(this, FogColorEvent, &FogColor::onGetFogColor)
    Module::onDisable();
}

void FogColor::defaultConfig() {
    settings.renameSetting("color", "colorOpacity", "color_rgb", "fog");
    Module::defaultConfig("core");
    setDef("fog", (std::string)"FFFFFF", 0.6f, false);
    
}

void FogColor::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Colors");
    addColorPicker("Fog Color", "", "fog");

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void FogColor::onGetFogColor(FogColorEvent &event) {
    if (!this->isEnabled()) return;
    D2D1_COLOR_F color = getColor("fog");

    event.setFogColorFromD2DColor(color, color.a);
}
