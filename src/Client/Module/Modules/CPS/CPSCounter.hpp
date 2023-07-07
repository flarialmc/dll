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

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"CPS: {value}");

        if (settings.getSettingByName<bool>("rightcps") == nullptr) {
            settings.addSetting("rightcps", false);
        }

        if(settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if(settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", true);
            settings.addSetting("borderWidth", 1.0f);
        }

        if(settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 40.0f);

        if(settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string)"120e0f");
            settings.addSetting("textColor", (std::string)"3f2a2d");
            settings.addSetting("borderColor", (std::string)"000000");
        }

        if(settings.getSettingByName<float>("bgOpacity") == nullptr) {
            settings.addSetting("bgOpacity", 1.0f);
            settings.addSetting("textOpacity", 1.0f);
            settings.addSetting("borderOpacity", 1.0f);
        }

        if(settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }

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

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, 40, 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(1.0, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(4, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
                                           toggleY,
                                           D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                           D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                           D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if(FlarialGUI::Toggle(0, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;



        FlarialGUI::FlarialText(toggleX + Constraints::SpacingConstraint(0.75, textWidth / 2.0f), toggleY, L"Border", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);


        percent = FlarialGUI::Slider(5, toggleX + Constraints::SpacingConstraint(1.2, textWidth) ,
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value, 4);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Border End */

        /* Rounding Start */
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        toggleX -= Constraints::SpacingConstraint(0.175, textWidth);

        FlarialGUI::FlarialText(toggleX , toggleY, L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.05f, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

        percent = FlarialGUI::Slider(6, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
                                     toggleY,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.30, textWidth);
        toggleX = Constraints::PercentageConstraint(0.019, "left");

        FlarialGUI::FlarialTextWithFont(toggleX+ Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Right CPS").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(2, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("rightcps")->value)) this->settings.getSettingByName<bool>("rightcps")->value = !this->settings.getSettingByName<bool>("rightcps")->value;

        /* Rounding End */

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY -= Constraints::SpacingConstraint(0.30, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Background").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(3, toggleX + Constraints::SpacingConstraint(0.95, textWidth), toggleY - Constraints::SpacingConstraint(0.017, textWidth), settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(4, toggleX + Constraints::SpacingConstraint(0.40, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Border").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(5, toggleX + Constraints::SpacingConstraint(0.55, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);
        /* Color Pickers End */

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(3, settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);
        FlarialGUI::ColorPickerWindow(4, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);
        FlarialGUI::ColorPickerWindow(5, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

    }
};