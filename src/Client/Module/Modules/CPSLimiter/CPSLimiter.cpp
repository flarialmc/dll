#include "CPSLimiter.hpp"

#include "Client.hpp"
#include "Manager.hpp"
#include "../../../GUI/Engine/Engine.hpp"

void CPSLimiter::defaultConfig() {
    settings.changeType<float, int>("Left");
    settings.changeType<float, int>("Right");
    Module::defaultConfig("core");
    setDef("Left", 16);
    setDef("Right", 24);
    setDef("debounce", 40); // 2 clicks at 25 CPS
}

void CPSLimiter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSliderInt("Left Click", "Limit for your LMB.", "Left");
    addSliderInt("Right Click", "Right for your RMB.", "Right");

    addSliderInt("Debounce Delay (ms)", "Minimum delay between clicks, in milliseconds.", "debounce", 200, 1);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}
