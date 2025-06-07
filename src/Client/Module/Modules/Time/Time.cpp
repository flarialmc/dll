#include "Time.hpp"

#include "Events/EventManager.hpp"

Time::Time(): Module("Clock", "Displays your current local time.", IDR_TIME_PNG, "")
{

    

}

void Time::onEnable()
{
    Listen(this, RenderEvent, &Time::onRender)
    Module::onEnable();
}

void Time::onDisable()
{
    Deafen(this, RenderEvent, &Time::onRender)
    Module::onDisable();
}

void Time::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    const auto now = std::time(nullptr);
    const std::tm calendarTime = localtime_xp(now);

    std::string meridiem;
    std::string seperator;

    int hour = calendarTime.tm_hour;
    int minute = calendarTime.tm_min;

    if (hour - 12 < 0) {
        meridiem = "AM";
    }
    else if (hour == 0) {
        hour = 12;
        meridiem = "AM";
    }
    else if (hour == 12) {
        hour = 12;
        meridiem = "PM";
    }
    else {
        meridiem = "PM";
        hour -= 12;
    }
    if (getOps<bool>("24") && meridiem == "PM") {
        hour += 12;
        meridiem = "";
    }
    else if (getOps<bool>("24") && meridiem == "AM") meridiem = "";

    seperator = minute < 10 ? ":0" : ":";

    if (hour == 24) hour = 0;

    std::string time = FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;

    this->normalRender(3, time);
}

void Time::defaultConfig()
{
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    setDef("24", false);
    Client::SaveSettings();
}

void Time::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));


    addHeader("Time");
    defaultAddSettings("main");
    addToggle("24 Hour Format", "", "24");
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
