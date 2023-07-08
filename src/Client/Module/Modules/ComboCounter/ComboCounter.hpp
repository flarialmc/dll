#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include  "ComboListener.hpp"

class ComboCounter : public Module {

public:

    ComboCounter() : Module("Combo", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\combo.png", 'Y') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"Combo: {value}");
        if(settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.70f);

        EventHandler::registerListener(new ComboListener("Combo", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("Combo");

        Module::onDisable();

    }

    void SettingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, 40, 2);
        FlarialGUI::SetScrollView(toggleX, toggleY, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"UI Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.12, "height", true));

        float percent = FlarialGUI::Slider(3, toggleX + Constraints::SpacingConstraint(0.65, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("uiscale")->value, 2.0f);

        this->settings.getSettingByName<float>("uiscale")->value = percent;


        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        if (FlarialGUI::Toggle(0, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("border")->value)) this->settings.getSettingByName<bool>("border")->value = !this->settings.getSettingByName<bool>("border")->value;



        FlarialGUI::FlarialText(toggleX + Constraints::SpacingConstraint(0.75, textWidth / 2.0f), toggleY, L"Border", D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);


        percent = FlarialGUI::Slider(1, toggleX + Constraints::SpacingConstraint(1.2, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("borderWidth")->value, 4);

        this->settings.getSettingByName<float>("borderWidth")->value = percent;

        /* Border End */

        /* Rounding Start */
        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        toggleX -= Constraints::SpacingConstraint(0.175, textWidth);

        FlarialGUI::FlarialText(toggleX, toggleY, L"Rounding", D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.05f, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

        percent = FlarialGUI::Slider(2, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
            toggleY,
            D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
            D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("rounding")->value);

        this->settings.getSettingByName<float>("rounding")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(0.60, textWidth), toggleY, FlarialGUI::to_wide("Translucency").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));

        if (FlarialGUI::Toggle(4, Constraints::PercentageConstraint(0.019, "left"), toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("BlurEffect")->value)) this->settings.getSettingByName<bool>("BlurEffect")->value = !this->settings.getSettingByName<bool>("BlurEffect")->value;
        if (this->settings.getSettingByName<bool>("BlurEffect")->value) {
            percent = FlarialGUI::Slider(7, Constraints::PercentageConstraint(0.019, "left") + Constraints::SpacingConstraint(1.65, textWidth),
                toggleY,
                D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("BlurIntensity")->value, 25);

            this->settings.getSettingByName<float>("BlurIntensity")->value = percent;
        }

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(5, settings.getSettingByName<std::string>("text")->value, 16, Constraints::PercentageConstraint(0.019, "left"), toggleY);

        FlarialGUI::FlarialText(toggleX , toggleY + Constraints::SpacingConstraint(0.35, textWidth), L"Text Scale", D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.05f, textHeight, DWRITE_TEXT_ALIGNMENT_CENTER);


        percent = FlarialGUI::Slider(8, toggleX + Constraints::SpacingConstraint(0.95, textWidth),
                                     toggleY + Constraints::SpacingConstraint(0.35, textWidth),
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                     D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("textscale")->value, 1.15);

        this->settings.getSettingByName<float>("textscale")->value = percent;

        /* Rounding End */

        /* Color Pickers Start*/

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY -= Constraints::SpacingConstraint(0.72, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Background").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(0, toggleX + Constraints::SpacingConstraint(0.95, textWidth), toggleY - Constraints::SpacingConstraint(0.017, textWidth), settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Text").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(1, toggleX + Constraints::SpacingConstraint(0.40, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, FlarialGUI::to_wide("Border").c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth * 1.4f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth));
        FlarialGUI::ColorPicker(2, toggleX + Constraints::SpacingConstraint(0.55, textWidth), toggleY * 0.99f, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(0, settings.getSettingByName<std::string>("bgColor")->value, settings.getSettingByName<float>("bgOpacity")->value);
        FlarialGUI::ColorPickerWindow(1, settings.getSettingByName<std::string>("textColor")->value, settings.getSettingByName<float>("textOpacity")->value);
        FlarialGUI::ColorPickerWindow(2, settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value);
        /* Color Pickers End */

    }
};