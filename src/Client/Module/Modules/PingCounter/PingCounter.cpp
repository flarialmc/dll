#include "PingCounter.hpp"

#include "Events/EventManager.hpp"

PingCounter::PingCounter(): Module("Ping Counter", "Displays your current latency to the server.",
                                   IDR_PING_PNG, "")
{
    
}

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
    setDef("text", (std::string)"{value}ms");
    setDef("textscale", 0.8f);
    Module::defaultConfig("all");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void PingCounter::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


    addHeader("Ping Counter");
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
    auto pingStr = FlarialGUI::cached_to_string(SDK::getServerPing());
    this->normalRender(11, pingStr);
}
