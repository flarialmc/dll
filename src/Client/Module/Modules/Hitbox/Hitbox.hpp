#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "HitboxListener.hpp"

class Hitbox : public Module {

public:

    Hitbox() : Module("Hitbox", "Displays hitboxes of entities", IDR_BLOCK_PNG, "") {

        Module::setup();

    };

    // TODO: add options to see all entities

    void onEnable() override {
        EventHandler::registerListener(new HitboxListener("Hitbox", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Hitbox");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);
        if (settings.getSettingByName<float>("thickness") == nullptr) settings.addSetting("thickness", 1.1f);
        if (settings.getSettingByName<bool>("staticThickness") == nullptr) settings.addSetting("staticThickness", false);
        if (settings.getSettingByName<bool>("outline") == nullptr) settings.addSetting("outline", false);
    }

    void settingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.60, textWidth), y,
                                        L"Outline", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(0, x, y, this->settings.getSettingByName<bool>(
                "outline")->value))
            this->settings.getSettingByName<bool>("outline")->value = !this->settings.getSettingByName<bool>(
                    "outline")->value;

        y += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.60, textWidth), y,
                                        L"Static thickness", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, x, y, this->settings.getSettingByName<bool>(
                "staticThickness")->value))
            this->settings.getSettingByName<bool>("staticThickness")->value = !this->settings.getSettingByName<bool>(
                    "staticThickness")->value;

        y += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(x, y, L"Thickness ", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float thickness = FlarialGUI::Slider(1, x + FlarialGUI::SettingsTextWidth("Thickness "),
                                            y, this->settings.getSettingByName<float>("thickness")->value, 5.0f,
                                            0.f, false);

        this->settings.getSettingByName<float>("thickness")->value = thickness;

        y += Constraints::SpacingConstraint(0.35, textWidth);

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

};
