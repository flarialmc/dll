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

        EventHandler::registerListener(new FPSListener("FPS", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("FPS");

        Module::onDisable();

    }

    void SettingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.02, "left");
        float toggleY = Constraints::PercentageConstraint(0.05, "top");

        D2D1_COLOR_F color = D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f);

        if (!this->settings.getSettingByName<bool>("border")->value) {
            color = D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f);
        }

        if(FlarialGUI::Toggle(0, toggleX, toggleY, color, D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;

        float percent = FlarialGUI::Slider(0, toggleX,
                           toggleY + Constraints::SpacingConstraint(0.35, Constraints::RelativeConstraint(0.6)),
                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

    }
};