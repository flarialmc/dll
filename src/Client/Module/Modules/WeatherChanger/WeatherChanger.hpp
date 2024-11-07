#pragma once

#include "../Module.hpp"


class WeatherChanger : public Module {
public:
    WeatherChanger() : Module("Weather Changer", "Changes the weather ingame.", IDR_CLOUDY_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, TickEvent, &WeatherChanger::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &WeatherChanger::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("rain") == nullptr) settings.addSetting("rain", 1.00f);

        if (settings.getSettingByName<float>("lighting") == nullptr) settings.addSetting("lighting", 0.00f);

        if (settings.getSettingByName<bool>("snow") == nullptr) settings.addSetting("snow", false);
    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("Rain Intensity", "", this->settings.getSettingByName<float>("rain")->value);
        this->addSlider("Snow Intensity", "", this->settings.getSettingByName<float>("snow")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onTick(TickEvent &event) {
        if (!SDK::clientInstance->getBlockSource())
            return;

        if (this->isEnabled()) {
            if (this->settings.getSettingByName<float>("rain")->value > 0.02f)
                SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = this->settings.getSettingByName<float>(
                        "rain")->value;
            else SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = 0.0f;
            if (this->settings.getSettingByName<float>("lighting")->value < 0.02f)
                SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = this->settings.getSettingByName<float>(
                        "lighting")->value;
            else SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = 0.0f;

            // TODO: When you set snow, it will stay even if on until game reload
            if (this->settings.getSettingByName<bool>("snow")->value) {
                Vec3<float> *pos = event.getActor()->getPosition();
                Vec3<int> e((int)pos->x, (int)pos->y, (int)pos->z);

                SDK::clientInstance->getBlockSource()->getBiome(e)->temparature = 0.0f;
            }
        }
    }
};

