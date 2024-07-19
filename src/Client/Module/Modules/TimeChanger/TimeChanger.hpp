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

    void settingsRender() override {


        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Time Slider", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("Time Slider "),
                                           toggleY, this->settings.getSettingByName<float>("time")->value, 1.0f);

        this->settings.getSettingByName<float>("time")->value = percent;

        FlarialGUI::UnsetScrollView();


    }
};