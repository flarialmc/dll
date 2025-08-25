#include "PingCounter.hpp"

#include "Events/EventManager.hpp"


void PingCounter::onEnable()
{
    Listen(this, RenderEvent, &PingCounter::onRender)
    Module::onEnable();
}

void PingCounter::onDisable()
{
    Deafen(this, RenderEvent, &PingCounter::onRender)
    Module::onDisable();
}

void PingCounter::defaultConfig()
{
    Module::defaultConfig("all");
    setDef("spoof", false);
    setDef("pingSpoofer", 1.0f);
    setDef("text", (std::string)"{value}ms");
    setDef("textscale", 0.8f);
}

void PingCounter::settingsRender(float settingsOffset)
{
    initSettingsPage();


    addHeader("Ping Counter");
    addToggle("Spoof Ping", "", "spoof");
    addConditionalSlider(getOps<bool>("spoof"), "Spoofer", "Adjusts the displayed ping.", "pingSpoofer", 10.0f);
    extraPadding();
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void PingCounter::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;

    float ping = SDK::getServerPing();

    if (getOps<bool>("spoof")) ping *= round(getOps<float>("pingSpoofer"));

    auto pingStr = FlarialGUI::cached_to_string((int)round(ping));

    this->normalRender(11, pingStr);
}
