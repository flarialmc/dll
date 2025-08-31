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
    initSettingsPage();

    addHeader("CPS Limiter");
    addSliderInt("Left Click", "Limit for your LMB.", "Left");
    addSliderInt("Right Click", "Right for your RMB.", "Right");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
