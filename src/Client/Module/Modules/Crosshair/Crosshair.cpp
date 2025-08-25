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
    
}

void Crosshair::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Crosshair");
    addToggle("Third Person Crosshair", "", "thirdpersoncrosshair");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
