#include "Crosshair.hpp"

void Crosshair::onEnable() {
    Module::onEnable();
}

void Crosshair::onDisable() {
    Module::onDisable();
}

void Crosshair::defaultConfig() {
    Module::defaultConfig("core");
    setDef("thirdpersoncrosshair", true);
    saveSettings();
}

void Crosshair::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Crosshair");
    addToggle("Third Person Crosshair", "", "thirdpersoncrosshair");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
