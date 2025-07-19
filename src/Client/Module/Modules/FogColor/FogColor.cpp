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
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

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
