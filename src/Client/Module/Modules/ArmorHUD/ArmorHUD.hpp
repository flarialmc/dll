#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ArmorHUDListener.hpp"

class ArmorHUD : public Module {

public:

    ArmorHUD() : Module("ArmorHUD", "Displays the armor you're\ncurrently wearing.",
                        IDR_CHESTPLATE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new ArmorHUDListener("ArmorHUD", this));
        if(FlarialGUI::inMenu){
            FlarialGUI::Notify("To change the position of ArmorHUD, Please click " +
                               ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                       "editmenubind")->value + " in the settings tab.");
        }
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("ArmorHUD");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 2.0f);
        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }
        if (settings.getSettingByName<bool>("vertical") == nullptr) settings.addSetting("vertical", false);
        if (settings.getSettingByName<bool>("percent") == nullptr) settings.addSetting("percent", false);
        if (settings.getSettingByName<bool>("color") == nullptr) settings.addSetting("color", false);
        if (settings.getSettingByName<bool>("showdurability") == nullptr) settings.addSetting("showdurability", false);

        if (settings.getSettingByName<std::string>("colorMain") == nullptr)
            settings.addSetting("colorMain", (std::string) "FFFFFF");

        if (settings.getSettingByName<bool>("colorMain_rgb") == nullptr) settings.addSetting("colorMain_rgb", false);

        if (settings.getSettingByName<std::string>("colorFull") == nullptr)
            settings.addSetting("colorFull", (std::string) "40FF00");

        if (settings.getSettingByName<bool>("colorFull_rgb") == nullptr) settings.addSetting("colorFull_rgb", false);

        if (settings.getSettingByName<std::string>("colorLow") == nullptr)
            settings.addSetting("colorLow", (std::string) "FF0000");

        if (settings.getSettingByName<bool>("colorLow_rgb") == nullptr) settings.addSetting("colorLow_rgb", false);

        if (settings.getSettingByName<float>("textSize") == nullptr) settings.addSetting("textSize", 0.12f);

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

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Vertical", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(2, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "vertical")->value))
            this->settings.getSettingByName<bool>("vertical")->value = !this->settings.getSettingByName<bool>(
                    "vertical")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Show durability", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(3, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "showdurability")->value))
            this->settings.getSettingByName<bool>("showdurability")->value = !this->settings.getSettingByName<bool>(
                    "showdurability")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Use percent %", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "percent")->value))
            this->settings.getSettingByName<bool>("percent")->value = !this->settings.getSettingByName<bool>(
                    "percent")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Change color", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(5, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "color")->value))
            this->settings.getSettingByName<bool>("color")->value = !this->settings.getSettingByName<bool>(
                    "color")->value;

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

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"High durability", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("High durability "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("colorFull")->value,
                                settings.getSettingByName<bool>("colorFull_rgb")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Low durability", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Low durability "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("colorLow")->value,
                                settings.getSettingByName<bool>("colorLow_rgb")->value);

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("colorMain")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("colorMain_rgb")->value);
        FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("colorFull")->value,
                                      settings.getSettingByName<float>("textOpacity")->value,
                                      settings.getSettingByName<bool>("colorFull_rgb")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("colorLow")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("colorLow_rgb")->value);
        /* Color Pickers End */
    }
};