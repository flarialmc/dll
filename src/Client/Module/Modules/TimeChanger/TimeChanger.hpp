#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class TimeChanger : public Module {

public:

    TimeChanger() : Module("Time Changer", "Changes the ingame time.", IDR_TIME_PNG, "") {

        Module::setup();

    };

    void defaultConfig() override {
        if (settings.getSettingByName<float>("time") == nullptr) settings.addSetting("time", 0.5f);
    }

    void settingsRender() override { }
};