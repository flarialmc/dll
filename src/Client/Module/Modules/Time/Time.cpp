#include "Time.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

Time::Time(): Module("Clock", "Displays your current local or ingame time.", IDR_TIME_PNG, "")
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

void Time::onSetup() {
    Listen(this, TimeEvent, &Time::onTimeEvent)
}

void Time::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;

    std::string text;

    if (getOps<bool>("igTime")) {
        text = formatMCTime(Time::curTime, getOps<bool>("24"));
    }
    else {
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

        text = FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;

        if (getOps<bool>("showDate")) {
            struct tm date;
            localtime_s(&date, &now);
            std::string dateStr;

            int sub;
            if (getOps<bool>("4letterYear")) sub = 1900;
            else sub = -100;

            if (getOps<bool>("donaldTrumpMode")) dateStr = std::format("{}/{}/{}", date.tm_mon + 1, date.tm_mday, date.tm_year + sub);
            else dateStr = std::format("{}/{}/{}", date.tm_mday, date.tm_mon + 1, date.tm_year + sub);

            if (getOps<bool>("timeBeforeDate")) text += "\n" + dateStr;
            else text = dateStr + "\n" + text;
        }

    }

    this->normalRender(3, text);
}

void Time::defaultConfig()
{
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    setDef("24", false);
    setDef("igTime", false);
    setDef("showDate", true);
    setDef("4letterYear", false);
    setDef("timeBeforeDate", false);
    setDef("donaldTrumpMode", false);
    
}

void Time::settingsRender(float settingsOffset)
{
    initSettingsPage();


    addHeader("Main");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Module Settings");
    addToggle("24 Hour Format", "", "24");
    addToggle("In Game Time Mode", "", "igTime");
    addConditionalToggle(!getOps<bool>("igTime"), "Show Date", "", "showDate");
    addConditionalToggle(!getOps<bool>("igTime"), "4 Letter Year", "2025 instead of 25", "4letterYear");
    addConditionalToggle(!getOps<bool>("igTime"), "Show Time Before Date", "", "timeBeforeDate");
    addConditionalToggle(!getOps<bool>("igTime") && getOps<bool>("showDate"), "mm/dd/yyyy mode", "", "donaldTrumpMode");
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

void Time::onTimeEvent(TimeEvent& event) {
    Time::curTime = event.getTime();
}

std::string Time::formatMCTime(float time, bool military) {
    int hour = floor(time * 24) - 12;
    while (hour <= 0) hour += 12;
    int minute = floor(static_cast<int>(time * 1440) % 60);

    if (military && time < 0.5f) hour += 12;

    return std::format("{}:{:02} {}", hour, minute, military ? "" : time < 0.5f ? "PM" : "AM");
}