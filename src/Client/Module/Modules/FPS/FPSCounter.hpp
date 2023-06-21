#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FPSListener.hpp"

class FPSCounter : public Module {

public:

    FPSCounter() : Module("FPS", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\fps.png", 'L') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if(settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if(settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", true);
            settings.addSetting("borderWidth", 5.0f);
        }

        if(settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 40.0f);

        EventHandler::registerListener(new FPSListener("FPS", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("FPS");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.12, "top");

        D2D1_COLOR_F color = D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f);

        if (!this->settings.getSettingByName<bool>("border")->value) {
            color = D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f);
        }

        if(FlarialGUI::Toggle(0, toggleX, toggleY, color, D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialText(toggleX + Constraints::SpacingConstraint(0.39, textWidth / 2.0f), toggleY, L"Border", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);


        float percent = FlarialGUI::Slider(1, toggleX + Constraints::SpacingConstraint(1.2, textWidth) ,
                           toggleY,
                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Border End */

        /* Rounding Start */

        toggleX -= Constraints::SpacingConstraint(0.325, textWidth);
        FlarialGUI::FlarialText(toggleX , toggleY + Constraints::SpacingConstraint(0.35, textWidth), L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.05f, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

        percent = FlarialGUI::Slider(2, toggleX + Constraints::SpacingConstraint(1.15, textWidth),
                                           toggleY + Constraints::SpacingConstraint(0.35, textWidth),
                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        /* Rounding End */

    }
};