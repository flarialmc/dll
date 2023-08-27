#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "CPSListener.hpp"


class CPSCounter : public Module {

public:


    CPSCounter() : Module("CPS", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\cursor.png", 'L') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<bool>("rightcps") == nullptr) {
            settings.addSetting("rightcps", false);
        }

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"CPS: {value}");


        if(settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);

        EventHandler::registerListener(new CPSListener("CPS", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("CPS");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if(FlarialGUI::Toggle(0, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;



        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, L"Border", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));


        percent = FlarialGUI::Slider(5, toggleX + Constraints::SpacingConstraint(1.2, textWidth) ,
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value, 4);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Border End */

        /* Rounding Start */
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(6, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX+ Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Right CPS").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(2, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("rightcps")->value)) this->settings.getSettingByName<bool>("rightcps")->value = !this->settings.getSettingByName<bool>("rightcps")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Translucency").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(4, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("BlurEffect")->value)) this->settings.getSettingByName<bool>("BlurEffect")->value = !this->settings.getSettingByName<bool>("BlurEffect")->value;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(7, settings.getSettingByName<std::string>("text")->value, 16, toggleX, toggleY);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(8, toggleX + FlarialGUI::SettingsTextWidth("Text Scale "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("textscale")->value, 2.00);

        this->settings.getSettingByName<float>("textscale")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        std::string txtAlignment = FlarialGUI::Dropdown(1,
                                                        toggleX, toggleY,
                                                        D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                                        D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                                        std::vector<std::string>{"Left", "Center", "Right"},
                                                        this->settings.getSettingByName<std::string>("textalignment")->value,
                                                        "Text Alignment"
        );

        this->settings.getSettingByName<std::string>("textalignment")->value = txtAlignment;

        FlarialGUI::SetIsInAdditionalYMode();

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Reverse Padding X").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(15, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("reversepaddingx")->value)) this->settings.getSettingByName<bool>("reversepaddingx")->value = !this->settings.getSettingByName<bool>("reversepaddingx")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Reverse Padding Y").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(16, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("reversepaddingy")->value)) this->settings.getSettingByName<bool>("reversepaddingy")->value = !this->settings.getSettingByName<bool>("reversepaddingy")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);


        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding X", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(15, toggleX + FlarialGUI::SettingsTextWidth("Padding X "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("padx")->value, 0.53f, 0.0f, false);

        this->settings.getSettingByName<float>("padx")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Padding Y", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(16, toggleX + FlarialGUI::SettingsTextWidth("Padding Y "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("pady")->value, 0.53f, 0.0f, false);

        this->settings.getSettingByName<float>("pady")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Width", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(17, toggleX + FlarialGUI::SettingsTextWidth("Rect Width "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rectwidth")->value, 3.0f);

        this->settings.getSettingByName<float>("rectwidth")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rect Height", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        percent = FlarialGUI::Slider(18, toggleX + FlarialGUI::SettingsTextWidth("Rect Height "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rectheight")->value, 3.0f);

        this->settings.getSettingByName<float>("rectheight")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Responsive Rectangle (Moves with the Text)").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 5.f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(17, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("responsivewidth")->value)) this->settings.getSettingByName<bool>("responsivewidth")->value = !this->settings.getSettingByName<bool>("responsivewidth")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rotation", D2D1::ColorF(D2D1::ColorF::White), textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));
        percent = FlarialGUI::Slider(19, toggleX + FlarialGUI::SettingsTextWidth("Rotation "),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rotation")->value, 359.0f, 0.0f, false);

        this->settings.getSettingByName<float>("rotation")->value = percent;

        FlarialGUI::UnSetIsInAdditionalYMode();

        /* Rounding End */

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Background").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(0, toggleX + FlarialGUI::SettingsTextWidth("Background "), toggleY - Constraints::SpacingConstraint(0.017, textWidth), settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("Text "), toggleY * 0.99f, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Border").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Border "), toggleY * 0.99f, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);
        FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

    }
};