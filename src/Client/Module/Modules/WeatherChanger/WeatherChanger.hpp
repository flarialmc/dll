#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "WeatherListener.hpp"


class WeatherChanger : public Module {

public:


    WeatherChanger() : Module("Weather Changer", "Changes the weather ingame.", IDR_CLOUDY_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new WeatherListener("Weather", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Weather");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("rain") == nullptr) settings.addSetting("rain", 1.00f);

        if (settings.getSettingByName<float>("lighting") == nullptr) settings.addSetting("lighting", 0.00f);

        if (settings.getSettingByName<bool>("snow") == nullptr) settings.addSetting("snow", false);
    }

    void settingsRender() override { }
    }
};

