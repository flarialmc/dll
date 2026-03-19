#include "PingCounter.hpp"

std::string PingCounter::getDisplayValue() {
    float ping = getOps<bool>("useLastPing") ? SDK::getLastPing() : SDK::getServerPing();

    if (getOps<bool>("spoof")) ping *= round(getOps<float>("pingSpoofer"));

    return FlarialGUI::cached_to_string((int)round(ping));
}

void PingCounter::customConfig() {
    setDef("text", (std::string)"{value}ms");
    setDef("textscale", 0.8f);
    setDef("useLastPing", false);
    setDef("spoof", false);
    setDef("pingSpoofer", 1.0f);
}

void PingCounter::customSettings() {
    addToggle("Use Last Ping", "Uses the last ping sample instead of the average.", "useLastPing");
    addToggle("Spoof Ping", "", "spoof");
    addConditionalSlider(getOps<bool>("spoof"), "Spoofer", "Adjusts the displayed ping.", "pingSpoofer", 10.0f);
    extraPadding();
}
