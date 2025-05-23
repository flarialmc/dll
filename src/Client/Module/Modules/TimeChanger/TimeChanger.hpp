#pragma once

#include "../Module.hpp"


class TimeChanger : public Module {
public:
    TimeChanger() : Module("Time Changer", "Changes the ingame time.", IDR_TIME_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, TimeEvent, &TimeChanger::onTimeEvent)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TimeEvent, &TimeChanger::onTimeEvent)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<float>("time") == nullptr) settings.addSetting("time", 0.5f);
    }

    void settingsRender(float settingsOffset) override {
        /* Border Start */

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Settings");
        this->addSlider("Time Slider", "", this->settings.getSettingByName<float>("time")->value, 1.f, 0.f, true);
       
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onTimeEvent(TimeEvent& event) {
        event.setTime(this->settings.getSettingByName<float>("time")->value);
    }
};