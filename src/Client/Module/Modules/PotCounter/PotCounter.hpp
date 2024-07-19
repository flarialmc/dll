#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "PotListener.hpp"

class PotCounter : public Module {

public:

    PotCounter() : Module("Pot Counter", "Counts how much potions are\nin your inventory.",
                          IDR_POTION_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new PotListener("Pot", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Pot");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} Pots");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);
    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(3, toggleX + FlarialGUI::SettingsTextWidth("UI Scale "),
                                           toggleY, this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if (FlarialGUI::Toggle(0, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "border")->value))
            this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>(
                    "border")->value;


        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"Border",
                                        textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        percent = FlarialGUI::Slider(1, toggleX + FlarialGUI::SettingsTextWidth("Border ") +
                                        Constraints::SpacingConstraint(0.60, textWidth),
                                     toggleY, this->settings.getSettingByName<float>("borderWidth")->value, 4);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Border End */

        /* Rounding Start */
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rounding", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(2, toggleX + FlarialGUI::SettingsTextWidth("Rounding "),
                                     toggleY, this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Translucency", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "BlurEffect")->value))
            this->settings.getSettingByName<bool>("BlurEffect")->value = !this->settings.getSettingByName<bool>(
                    "BlurEffect")->value;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 16, toggleX, toggleY);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text Scale", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(8, toggleX + FlarialGUI::SettingsTextWidth("Text Scale "),
                                     toggleY, this->settings.getSettingByName<float>("textscale")->value, 2.00);

        this->settings.getSettingByName<float>("textscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        std::string txtAlignment = FlarialGUI::Dropdown(1,
                                                        toggleX, toggleY,
                                                        std::vector<std::string>{"Left", "Center", "Right"},
                                                        this->settings.getSettingByName<std::string>(
                                                                "textalignment")->value,
                                                        "Text Alignment"
        );

        this->settings.getSettingByName<std::string>("textalignment")->value = txtAlignment;

        FlarialGUI::SetIsInAdditionalYMode();

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Reverse Padding X", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(15, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "reversepaddingx")->value))
            this->settings.getSettingByName<bool>("reversepaddingx")->value = !this->settings.getSettingByName<bool>(
                    "reversepaddingx")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Reverse Padding Y", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(16, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "reversepaddingy")->value))
            this->settings.getSettingByName<bool>("reversepaddingy")->value = !this->settings.getSettingByName<bool>(
                    "reversepaddingy")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding X", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(15, toggleX + FlarialGUI::SettingsTextWidth("Padding X "),
                                     toggleY, this->settings.getSettingByName<float>("padx")->value, 0.53f, 0.0f,
                                     false);

        this->settings.getSettingByName<float>("padx")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding Y", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(16, toggleX + FlarialGUI::SettingsTextWidth("Padding Y "),
                                     toggleY, this->settings.getSettingByName<float>("pady")->value, 0.53f, 0.0f,
                                     false);

        this->settings.getSettingByName<float>("pady")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Width", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(17, toggleX + FlarialGUI::SettingsTextWidth("Rect Width "),
                                     toggleY, this->settings.getSettingByName<float>("rectwidth")->value, 3.0f);

        this->settings.getSettingByName<float>("rectwidth")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Height", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        percent = FlarialGUI::Slider(18, toggleX + FlarialGUI::SettingsTextWidth("Rect Height "),
                                     toggleY, this->settings.getSettingByName<float>("rectheight")->value, 3.0f);

        this->settings.getSettingByName<float>("rectheight")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Responsive Rectangle (Moves with the Text)",
                                        textWidth * 5.f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(17, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "responsivewidth")->value))
            this->settings.getSettingByName<bool>("responsivewidth")->value = !this->settings.getSettingByName<bool>(
                    "responsivewidth")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rotation", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        percent = FlarialGUI::Slider(19, toggleX + FlarialGUI::SettingsTextWidth("Rotation "),
                                     toggleY, this->settings.getSettingByName<float>("rotation")->value, 359.0f, 0.0f,
                                     false);

        this->settings.getSettingByName<float>("rotation")->value = percent;

        FlarialGUI::UnSetIsInAdditionalYMode();

        /* Rounding End */

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Background", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Background "),
                                toggleY - Constraints::SpacingConstraint(0.017, textWidth),
                                settings.getSettingByName<std::string>("bgColor")->value,
                                settings.getSettingByName<bool>("bgRGB")->value);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("Text "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("textColor")->value,
                                settings.getSettingByName<bool>("textRGB")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Border", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Border "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("borderColor")->value,
                                settings.getSettingByName<bool>("borderRGB")->value);

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("bgColor")->value,
                                      settings.getSettingByName<float>("bgOpacity")->value,
                                      settings.getSettingByName<bool>("bgRGB")->value);
        FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("textColor")->value,
                                      settings.getSettingByName<float>("textOpacity")->value,
                                      settings.getSettingByName<bool>("textRGB")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("borderColor")->value,
                                      settings.getSettingByName<float>("borderOpacity")->value,
                                      settings.getSettingByName<bool>("borderRGB")->value);
        /* Color Pickers End */

    }
};