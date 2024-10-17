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

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("Rain Intensity", "", this->settings.getSettingByName<float>("rain")->value);
        this->addSlider("Snow Intensity", "", this->settings.getSettingByName<float>("snow")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};

