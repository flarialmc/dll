#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "IPDisplayListener.hpp"


class IPDisplay : public Module{

public:


    IPDisplay() : Module("IP Display", "oAnshul==bari", "\\Flarial\\assets\\memory.png", NULL) {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<bool>("BlurEffect") == nullptr) {
            settings.addSetting("BlurEffect", false);
            settings.addSetting("BlurIntensity", 7.5f);
        }

        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value}");

        if (settings.getSettingByName<float>("percentageX") == nullptr) {
            settings.addSetting("percentageX", 0.0f);
            settings.addSetting("percentageY", 0.0f);
        }

        if (settings.getSettingByName<bool>("border") == nullptr) {
            settings.addSetting("border", true);
            settings.addSetting("borderWidth", 1.0f);
        }

        if (settings.getSettingByName<float>("rounding") == nullptr) settings.addSetting("rounding", 40.0f);

        if (settings.getSettingByName<std::string>("bgColor") == nullptr) {
            settings.addSetting("bgColor", (std::string)"120e0f");
            settings.addSetting("textColor", (std::string)"3f2a2d");
            settings.addSetting("borderColor", (std::string)"000000");
        }

        if (settings.getSettingByName<float>("bgOpacity") == nullptr) {
            settings.addSetting("bgOpacity", 1.0f);
            settings.addSetting("textOpacity", 1.0f);
            settings.addSetting("borderOpacity", 1.0f);
        }

        if (settings.getSettingByName<float>("uiscale") == nullptr) {

            settings.addSetting("uiscale", 0.65f);
        }

        if(settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.0f);

        EventHandler::registerListener(new IPDisplayListener("IPDisplay", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("IPDisplay");

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
            percent = FlarialGUI::Slider(7, Constraints::PercentageConstraint(0.019, "left") + + Constraints::SpacingConstraint(1.65, textWidth),
                toggleY,
                D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f),
                D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<float>("BlurIntensity")->value, 25);

            this->settings.getSettingByName<float>("BlurIntensity")->value = percent;
        }

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::TextBoxVisual(7, settings.getSettingByName<std::string>("text")->value, 16, Constraints::PercentageConstraint(0.019, "left"), toggleY);

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

    void NormalRender(int index, std::string text, std::string value) override {

        std::string uppercaseSentence;
        std::string search = "{VALUE}";

        for (char c : text) {
            uppercaseSentence += std::toupper(c);
        }

        size_t pos = uppercaseSentence.find(search);
        if (pos != std::string::npos) {
            text.replace(pos, search.length(), value);
        }

        float textWidth = Constraints::RelativeConstraint(0.7f * settings.getSettingByName<float>("uiscale")->value);
        float textHeight = Constraints::RelativeConstraint(0.1f * settings.getSettingByName<float>("uiscale")->value);


        Vec2<float> settingperc = Vec2<float>(this->settings.getSettingByName<float>("percentageX")->value,
            this->settings.getSettingByName<float>("percentageY")->value);

        Vec2<float> realcenter;

        if (settingperc.x != 0)
            realcenter = Vec2<float>(settingperc.x * MC::windowSize.x,
                settingperc.y * MC::windowSize.y);
        else
            realcenter = Constraints::CenterConstraint(textWidth, textHeight);

        float rectWidth = Constraints::RelativeConstraint(0.120f * settings.getSettingByName<float>("uiscale")->value);
        Vec2<float> rounde = Constraints::RoundingConstraint(this->settings.getSettingByName<float>("rounding")->value * settings.getSettingByName<float>("uiscale")->value, this->settings.getSettingByName<float>("rounding")->value * settings.getSettingByName<float>("uiscale")->value);

        float textSize = Constraints::SpacingConstraint(2.3f, rectWidth);

        float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

        IDWriteFactory *writeFactory;

        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                            reinterpret_cast<IUnknown **>(&writeFactory));
        IDWriteTextFormat *textFormat;
        writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(textSize), L"", &textFormat);
        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        IDWriteTextLayout *textLayout;

        writeFactory->CreateTextLayout(
                FlarialGUI::to_wide(text).c_str(),
                wcslen(FlarialGUI::to_wide(text).c_str()),
                textFormat,
                textWidth,
                textHeight,
                &textLayout
        );

        DWRITE_TEXT_METRICS textMetrics;
        textLayout->GetMetrics(&textMetrics);

        textLayout->Release();
        writeFactory->Release();
        textFormat->Release();

        rectWidth = textMetrics.width + Constraints::SpacingConstraint(2.0, realspacing);

        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, textHeight, index);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, index, rectWidth, textHeight);

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;


        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        D2D1_COLOR_F bgColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("bgColor")->value);
        D2D1_COLOR_F textColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("textColor")->value);
        D2D1_COLOR_F borderColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("borderColor")->value);

        bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
        textColor.a = settings.getSettingByName<float>("textOpacity")->value;
        borderColor.a = settings.getSettingByName<float>("borderOpacity")->value;

        if(settings.getSettingByName<bool>("BlurEffect")->value) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(realcenter.x, realcenter.y, realcenter.x + rectWidth, realcenter.y + textHeight), rounde.x, rounde.x), settings.getSettingByName<float>("BlurIntensity")->value);

                    FlarialGUI::RoundedRect(realcenter.x, realcenter.y,
            bgColor, rectWidth, textHeight,
            rounde.x, rounde.x);

        FlarialGUI::FlarialTextWithFont(realcenter.x + realspacing, realcenter.y,
            FlarialGUI::to_wide(text).c_str(),
            textColor, textWidth,
            textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, textSize);

        if (this->settings.getSettingByName<bool>("border")->value) {
            FlarialGUI::RoundedHollowRect(realcenter.x, realcenter.y, Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value * settings.getSettingByName<float>("uiscale")->value) / 100.0f, "height", true),
                borderColor, rectWidth, textHeight,
                rounde.x, rounde.x);
        }

        FlarialGUI::UnsetWindowRect();

    }

};

