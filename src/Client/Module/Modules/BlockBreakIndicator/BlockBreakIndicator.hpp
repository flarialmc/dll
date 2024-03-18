#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "BlockBreakIndicatorListener.hpp"
#include "../CPS/CPSListener.hpp"
#include "../../../Client.hpp"

class BlockBreakIndicator : public Module {

public:


    BlockBreakIndicator() : Module("Break Progress", "Visual indicator to show the progress\nof breaking a block.",
                                   R"(\Flarial\assets\block_break_indicator.png)", "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new BlockBreakIndicatorListener("BlockBreakIndicatorListener", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("BlockBreakIndicatorListener");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);
        if (settings.getSettingByName<bool>("pbmode") == nullptr) settings.addSetting("pbmode", true);
        if (settings.getSettingByName<bool>("onlyShowWhileBreaking") == nullptr)
            settings.addSetting("onlyShowWhileBreaking", false);
        if (settings.getSettingByName<std::string>("orientation") == nullptr)
            settings.addSetting("orientation", (std::string) "Vertical");
        if (settings.getSettingByName<float>("pbwidth") == nullptr) settings.addSetting("pbwidth", 0.91f);
        if (settings.getSettingByName<float>("pbheight") == nullptr) settings.addSetting("pbheight", 0.82f);

        if (settings.getSettingByName<std::string>("barFill") == nullptr)
            settings.addSetting("barFill", (std::string) "a83232");
        if (settings.getSettingByName<float>("barFillOpacity") == nullptr) settings.addSetting("barFillOpacity", 1.0f);
        if (settings.getSettingByName<bool>("barFillRGB") == nullptr) settings.addSetting("barFillRGB", false);

    }

    void settingsRender() override {

        /* Border Start */

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
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
        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Progress bar mode", textWidth * 5.f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(18, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "pbmode")->value))
            this->settings.getSettingByName<bool>("pbmode")->value = !this->settings.getSettingByName<bool>(
                    "pbmode")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Only Show Bar only when breaking",
                                        textWidth * 5.f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(19, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "onlyShowWhileBreaking")->value))
            this->settings.getSettingByName<bool>(
                    "onlyShowWhileBreaking")->value = !this->settings.getSettingByName<bool>(
                    "onlyShowWhileBreaking")->value;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        std::string orientation = FlarialGUI::Dropdown(2,
                                                       toggleX, toggleY,
                                                       std::vector<std::string>{"Horizontal", "Vertical"},
                                                       this->settings.getSettingByName<std::string>(
                                                               "orientation")->value,
                                                       "Orientation"
        );

        this->settings.getSettingByName<std::string>("orientation")->value = orientation;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Bar width", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        percent = FlarialGUI::Slider(19, toggleX + FlarialGUI::SettingsTextWidth("Bar width "),
                                     toggleY, this->settings.getSettingByName<float>("pbwidth")->value, 2.0f);

        this->settings.getSettingByName<float>("pbwidth")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Bar height", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        percent = FlarialGUI::Slider(20, toggleX + FlarialGUI::SettingsTextWidth("Bar height "),
                                     toggleY, this->settings.getSettingByName<float>("pbheight")->value, 2.0f);

        this->settings.getSettingByName<float>("pbheight")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Rotation", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        percent = FlarialGUI::Slider(21, toggleX + FlarialGUI::SettingsTextWidth("Rotation "),
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
                                settings.getSettingByName<float>("bgOpacity")->value,
                                settings.getSettingByName<bool>("bgRGB")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Text", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(1, toggleX + FlarialGUI::SettingsTextWidth("Text "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("textColor")->value,
                                settings.getSettingByName<float>("textOpacity")->value,
                                settings.getSettingByName<bool>("textRGB")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Border", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(2, toggleX + FlarialGUI::SettingsTextWidth("Border "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("borderColor")->value,
                                settings.getSettingByName<float>("borderOpacity")->value,
                                settings.getSettingByName<bool>("borderRGB")->value);

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Bar Fill", textWidth * 6.9f,
                                        textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(3, toggleX + FlarialGUI::SettingsTextWidth("Bar Fill "), toggleY * 0.99f,
                                settings.getSettingByName<std::string>("barFill")->value,
                                settings.getSettingByName<float>("barFillOpacity")->value,
                                settings.getSettingByName<bool>("barFillRGB")->value);

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
        FlarialGUI::ColorPickerWindow(3, settings.getSettingByName<std::string>("barFill")->value,
                                      settings.getSettingByName<float>("barFillOpacity")->value,
                                      settings.getSettingByName<bool>("barFillRGB")->value);
        /* Color Pickers End */

    }

    float currentHeight = 0.0f;

    void normalRender(int index, std::string& value) override {
        if(SDK::currentScreen != "hud_screen") return;
        // TODO: needs a logic where if you stopped holding left button it wont show progress if its higher than 0% (if u resetted mining you start at 0%)
        if (!CPSListener::GetLeftHeld()) {
            value = "0%";
        }

        if (settings.getSettingByName<bool>("pbmode")->value) {
            if (settings.getSettingByName<bool>("onlyShowWhileBreaking")->value && value == "0%") return;

            value.pop_back();
            float percent;
            std::istringstream(value) >> percent;
            percent /= 100.0f;

            float barheight = Constraints::RelativeConstraint(
                    0.5f * settings.getSettingByName<float>("uiscale")->value);
            float barwidth = Constraints::RelativeConstraint(
                    0.05f * settings.getSettingByName<float>("uiscale")->value);

            D2D1_COLOR_F bgColor = settings.getSettingByName<bool>("bgRGB")->value ? FlarialGUI::rgbColor
                                                                                   : FlarialGUI::HexToColorF(
                            settings.getSettingByName<std::string>("bgColor")->value);
            D2D1_COLOR_F barFill = settings.getSettingByName<bool>("barFillRGB")->value ? FlarialGUI::rgbColor
                                                                                        : FlarialGUI::HexToColorF(
                            settings.getSettingByName<std::string>("barFill")->value);
            D2D1_COLOR_F outline = settings.getSettingByName<bool>("borderRGB")->value ? FlarialGUI::rgbColor
                                                                                       : FlarialGUI::HexToColorF(
                            settings.getSettingByName<std::string>("borderColor")->value);

            std::string orientation = settings.getSettingByName<std::string>("orientation")->value;

            bgColor.a = settings.getSettingByName<float>("bgOpacity")->value;
            barFill.a = settings.getSettingByName<float>("barFillOpacity")->value;
            outline.a = settings.getSettingByName<float>("borderOpacity")->value;

            Vec2<float> settingperc = Vec2<float>(
                    settings.getSettingByName<float>("percentageX")->value,
                    settings.getSettingByName<float>("percentageY")->value
            );

            Vec2<float> coord;
            Vec2<float> rounde = Constraints::RoundingConstraint(
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value,
                    this->settings.getSettingByName<float>("rounding")->value *
                    settings.getSettingByName<float>("uiscale")->value);

            if (settingperc.x != 0)
                coord = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
            else
                coord = Constraints::CenterConstraint(barwidth * settings.getSettingByName<float>("pbwidth")->value,
                                                      barheight * settings.getSettingByName<float>("pbheight")->value);

            float pbwidth = barwidth * settings.getSettingByName<float>("pbwidth")->value;
            float pbheight = barheight * this->settings.getSettingByName<float>("pbheight")->value;

            if (orientation == "Horizontal") {
                std::swap(pbwidth, pbheight);
            }

            if (ModuleManager::getModule("ClickGUI")->isEnabled() ||
                ClickGUIRenderer::editmenu) {
                FlarialGUI::SetWindowRect(coord.x, coord.y, pbwidth, pbheight, index);

                Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(coord.x, coord.y, index, pbwidth, pbheight);

                coord.x = vec2.x;
                coord.y = vec2.y;

                coord = coord;

                Vec2<float> percentages = Constraints::CalculatePercentage(coord.x, coord.y);

                this->settings.setValue("percentageX", percentages.x);
                this->settings.setValue("percentageY", percentages.y);
            }

            float filledHeight = (orientation == "Horizontal" ? pbwidth : pbheight) * percent;

            FlarialGUI::lerp(currentHeight, filledHeight, 0.5f * FlarialGUI::frameFactor);

            if (settings.getSettingByName<bool>("BlurEffect")->value)
                FlarialGUI::BlurRect(
                        D2D1::RoundedRect(D2D1::RectF(coord.x, coord.y, coord.x + pbwidth, coord.y + pbheight),
                                          rounde.x, rounde.x),
                        Client::settings.getSettingByName<float>("blurintensity")->value);

            FlarialGUI::RoundedRect(coord.x, coord.y, bgColor, pbwidth, pbheight, rounde.x, rounde.y);
            if (orientation == "Horizontal")
                FlarialGUI::RoundedRect(coord.x, coord.y, barFill, currentHeight, pbheight, rounde.x, rounde.y);
            else
                FlarialGUI::RoundedRect(coord.x, coord.y + pbheight - currentHeight, barFill, pbwidth, currentHeight,
                                        rounde.x, rounde.y);

            if (this->settings.getSettingByName<bool>("border")->value)
                FlarialGUI::RoundedHollowRect(
                        coord.x,
                        coord.y,
                        Constraints::RelativeConstraint((this->settings.getSettingByName<float>("borderWidth")->value *
                                                         settings.getSettingByName<float>("uiscale")->value) / 100.0f,
                                                        "height", true),
                        outline,
                        pbwidth,
                        pbheight,
                        rounde.x,
                        rounde.y
                );

            if (ModuleManager::getModule("ClickGUI")->isEnabled() ||
                ClickGUIRenderer::editmenu)
                FlarialGUI::UnsetWindowRect();
        } else Module::normalRender(index, value);
    }
};

