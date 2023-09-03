#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"

class CPSLimiter : public Module {

public:


    CPSLimiter() : Module("CPS Limiter", "Limit how many clicks you can\nregister per second.", "\\Flarial\\assets\\stop.png", 'o') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<float>("Left") == nullptr) settings.addSetting("Left", 16.0f);

        if (settings.getSettingByName<float>("Right") == nullptr) settings.addSetting("Right", 24.0f);

    }

    void onDisable() override {

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Left", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(4, toggleX + FlarialGUI::SettingsTextWidth("Left "),
                                           toggleY, this->settings.getSettingByName<float>("Left")->value, 100.0f,
                                           1.00f, 0);

        this->settings.getSettingByName<float>("Left")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Right", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(5, toggleX + FlarialGUI::SettingsTextWidth("Right "),
                                     toggleY, this->settings.getSettingByName<float>("Right")->value, 100.0f, 1.00f, 0);

        this->settings.getSettingByName<float>("Right")->value = percent;
    }
};

