#include "CPSLimiter.hpp"

void CPSLimiter::defaultConfig() {
    Module::defaultConfig("core");
    setDef("Left", 16.0f);
    setDef("Right", 24.0f);
    setDef("legacy", false);
    if (ModuleManager::initialized) Client::SaveSettings();
}

void CPSLimiter::settingsRender(float settingsOffset) {

    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("CPS Limiter");
    addSlider("Left Click", "Limit for your LMB.", "Left");
    addSlider("Right Click", "Right for your RMB.", "Right");
    addToggle("Legacy Mode", "An alternative mode for limiting cps, may not work as expected.", "legacy");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
