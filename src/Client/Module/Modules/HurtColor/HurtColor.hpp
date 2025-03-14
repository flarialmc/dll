#pragma once

#include "../Module.hpp"

class HurtColor : public Module {

public:
    HurtColor() : Module("Hurt Color", "Change the color when you hit entities.", IDR_HURT_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, HurtColorEvent, &HurtColor::onGetHurtColor)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.65);
    }

    void settingsRender(float settingsOffset) override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(x, y, L"Color", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(0, x + FlarialGUI::SettingsTextWidth("Color "),
                                y - Constraints::SpacingConstraint(0.017, textWidth),
                                settings.getSettingByName<std::string>("color")->value,
                                settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

    }

    void onGetHurtColor(HurtColorEvent &event) {
        D2D1_COLOR_F color;
        if (this->settings.getSettingByName<bool>("color_rgb")->value)
            color = FlarialGUI::rgbColor;
        else
            color = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color")->value);

        event.setHurtColorFromD2DColor(color, this->settings.getSettingByName<float>("colorOpacity")->value);
    }
};