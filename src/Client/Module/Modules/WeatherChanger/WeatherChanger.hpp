#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "WeatherListener.hpp"


class WeatherChanger : public Module {

public:


    WeatherChanger() : Module("Weather Changer", "Changes the weather ingame.", "\\Flarial\\assets\\cloudy.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<float>("rain") == nullptr) settings.addSetting("rain", 1.00f);

        if (settings.getSettingByName<float>("lighting") == nullptr) settings.addSetting("lighting", 0.00f);

        if (settings.getSettingByName<bool>("snow") == nullptr) settings.addSetting("snow", false);


        EventHandler::registerListener(new WeatherListener("Weather", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("Weather");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rain", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("Rain "),
                                           toggleY, this->settings.getSettingByName<float>("rain")->value, 10.0f);

        this->settings.getSettingByName<float>("rain")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Lightning", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(5, toggleX + FlarialGUI::SettingsTextWidth("Lightning "),
                                     toggleY, this->settings.getSettingByName<float>("lighting")->value, 10.0f);

        this->settings.getSettingByName<float>("lighting")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "snow")->value)) this->settings.getSettingByName<bool>("snow")->value = !this->settings.getSettingByName<bool>("snow")->value;
        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth / 2.0f), toggleY,
                                        L"Snow (intensity depends on rain)",
                                        Constraints::SpacingConstraint(4.5, textWidth), textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.95, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

    }
};

