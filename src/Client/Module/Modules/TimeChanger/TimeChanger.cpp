#include "TimeChanger.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

void TimeChanger::onEnable()
{
    Listen(this, TimeEvent, &TimeChanger::onTimeEvent)
    Module::onEnable();
}

void TimeChanger::onDisable()
{
    Deafen(this, TimeEvent, &TimeChanger::onTimeEvent)
    Module::onDisable();
}

void TimeChanger::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("time", 0.5f);
    
}

void TimeChanger::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Time Changer");
    addSlider("Time Slider", "", "time", 1.f, 0.f, true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void TimeChanger::onTimeEvent(TimeEvent& event)
{
    if (!this->isEnabled()) return;
    event.setTime(getOps<float>("time"));
}
