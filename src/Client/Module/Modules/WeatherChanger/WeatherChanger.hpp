#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "WeatherListener.hpp"


class WeatherChanger : public Module {

public:


    WeatherChanger() : Module("Weather Changer", "oAnshul==bari", "\\Flarial\\assets\\cloudy.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<float>("rain") == nullptr) settings.addSetting("rain", 0.00f);

        if (settings.getSettingByName<float>("lighting") == nullptr) settings.addSetting("lighting", 0.00f);

        EventHandler::registerListener(new WeatherListener("Weather", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("Time");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rain", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rain")->value, 10.0f);

        this->settings.getSettingByName<float>("rain")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Lighting", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(5, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("lighting")->value, 10.0f);

        this->settings.getSettingByName<float>("lighting")->value = percent;

    }
};

