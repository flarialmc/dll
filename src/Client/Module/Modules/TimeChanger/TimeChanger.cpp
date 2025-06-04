#include "TimeChanger.hpp"

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
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Time Changer");
    addSlider("Time Slider", "", "time", 1.f, 0.f, true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void TimeChanger::onTimeEvent(TimeEvent& event)
{
    event.setTime(getOps<float>("time"));
}
