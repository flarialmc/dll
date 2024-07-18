#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"

class CPSLimiter : public Module {

public:


    CPSLimiter() : Module("CPS Limiter", "Limit how many clicks you can\nregister per second.",
                          IDR_STOP_PNG, "") {

        Module::setup();
    };

    void defaultConfig() override {
        if (settings.getSettingByName<float>("Left") == nullptr) settings.addSetting("Left", 16.0f);

        if (settings.getSettingByName<float>("Right") == nullptr) settings.addSetting("Right", 24.0f);
    }

    void settingsRender() override { }
};

