#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "PotionHUDListener.hpp"

class PotionHUD : public Module {

public:

    PotionHUD() : Module("PotionHUD", "Displays your potion effects",
                        IDR_POTION_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new PotionHUDListener("PotionHUD", this));
        if(FlarialGUI::inMenu){
            FlarialGUI::Notify("To change the position of PotionHUD, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("PotionHUD");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.0f);

        if (settings.getSettingByName<std::string>("colorMain") == nullptr)
            settings.addSetting("colorMain", (std::string) "FFFFFF");

        if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

        if (settings.getSettingByName<std::string>("colorLow") == nullptr)
            settings.addSetting("colorLow", (std::string) "FF0000");

        if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);

        if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.14f);

    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY,
                                           this->settings.getSettingByName<float>("uiscale")->value, 3.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text size ", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float textSize = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Text size "),
                                           toggleY, this->settings.getSettingByName<float>("textSize")->value, 0.25f,
                                           0.f, false);

        this->settings.getSettingByName<float>("textSize")->value = textSize;

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Main color", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Main color "),
                                toggleY - Constraints::SpacingConstraint(0.017, textWidth),
                                settings.getSettingByName<std::string>("colorMain")->value,
                                settings.getSettingByName<bool>("colorMain_rgb")->value);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Effect about to expire", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Effect about to expire "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("colorLow")->value,
                                settings.getSettingByName<bool>("colorLow_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("colorMain")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("colorMain_rgb")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("colorLow")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("colorLow_rgb")->value);
        /* Color Pickers End */
    }
};