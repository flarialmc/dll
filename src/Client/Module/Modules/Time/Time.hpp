#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "TimeListener.hpp"


class Time : public Module {

public:


    Time() : Module("IRL Time", "Displays your current local time.", IDR_TIME_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new TimeListener("Time", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("Time");
        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("24") == nullptr) {
            settings.addSetting("24", false);
        }

        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");


        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
    }

    void settingsRender() override { }
};

