#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "HurtColorListener.hpp"


class HurtColor : public Module {

public:

    HurtColor() : Module("Hurt Color", "Change the color when you hit entities.", R"(\Flarial\assets\hurt.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new HurtColorListener("HurtColor", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("HurtColor");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.65);
    }

    void settingsRender() override {

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
                                settings.getSettingByName<float>("colorOpacity")->value,
                                settings.getSettingByName<bool>("color_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("color")->value,
                                      settings.getSettingByName<float>("colorOpacity")->value,
                                      settings.getSettingByName<bool>("color_rgb")->value);

    }
};