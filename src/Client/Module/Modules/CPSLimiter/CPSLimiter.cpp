#include "CPSLimiter.hpp"

#include "Client.hpp"
#include "Manager.hpp"

void CPSLimiter::defaultConfig() {
    settings.changeType<float, int>("Left");
    settings.changeType<float, int>("Right");
    Module::defaultConfig("core");
    setDef("Left", 16);
    setDef("Right", 24);
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
    addSliderInt("Left Click", "Limit for your LMB.", "Left");
    addSliderInt("Right Click", "Right for your RMB.", "Right");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
