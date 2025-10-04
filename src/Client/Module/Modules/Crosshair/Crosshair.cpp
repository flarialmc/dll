#include "Crosshair.hpp"
#include <GUI/Engine/EngineImGui.hpp>
#include "../../../GUI/Engine/Engine.hpp"

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

    addToggle("Third Person Crosshair", "", "thirdpersoncrosshair");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
