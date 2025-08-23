#include "Module.hpp"

#include "../../Client.hpp"
#include "../../Events/Events.hpp"
#include "ClickGUI/ClickGUI.hpp"
#include "Scripting/ScriptManager.hpp"
#include "../../../Utils/Telemetry.hpp"
#include <vector>
#include <cmath>

#define clickgui ModuleManager::getModule("ClickGUI")

static std::string Lname = "";

D2D_COLOR_F Module::getColor(std::string text) {
    D2D_COLOR_F col = this->getOps<bool>(text + "RGB") ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->getOps<std::string>(text + "Col"));
    col.a = this->getOps<float>(text + "Opacity");
    return col;
};

D2D_COLOR_F Module::getColor(std::string text, std::string mod) {
    auto lol = ModuleManager::getModule(mod);
    D2D_COLOR_F col = lol->getOps<bool>(text + "RGB") ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(lol->getOps<std::string>(text + "Col"));
    col.a = lol->getOps<float>(text + "Opacity");
    return col;
}

void Module::initSettingsPage() {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));
};

void Module::normalRenderCore(int index, std::string &text) {
    if (!this->isEnabled()) return;
    float rotation = getOps<float>("rotation");
    DWRITE_TEXT_ALIGNMENT alignment = alignments[getOps<std::string>("textalignment")];
    bool responsivewidth = getOps<bool>("responsivewidth");
    float paddingX = getOps<float>("padx");
    float paddingY = getOps<float>("pady");

    if (getOps<bool>("reversepaddingx")) paddingX = -(getOps<float>("padx"));
    if (getOps<bool>("reversepaddingy")) paddingY = -(getOps<float>("pady"));

    float textWidth = Constraints::RelativeConstraint(0.7f * getOps<float>("uiscale"));
    float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale"));
    float textSize = Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale");

    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

    float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

    ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
        FlarialGUI::to_wide(text).c_str(),
        1000000,
        textHeight,
        alignment,
        textSize, DWRITE_FONT_WEIGHT_NORMAL, true
    );

    float rectWidth = (
        !responsivewidth
            ? (Constraints::RelativeConstraint(0.225f * getOps<float>("uiscale")) * getOps<float>("rectwidth"))
            : (textMetrics.x + Constraints::SpacingConstraint(2.0, realspacing)) * getOps<float>("rectwidth")
    );
    float rectHeight = textHeight * getOps<float>("rectheight");

    Vec2<float> topleft;

    if (settingperc.x != 0) topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

    if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;

    if (prevAlignments[index] != alignment) {
        float toAdjust = 0;
        if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_CENTER) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_LEADING) toAdjust = rectWidth / -2.f;
            else toAdjust = rectWidth / 2.f;
        } else if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_CENTER) toAdjust = rectWidth / 2.f;
            else toAdjust = rectWidth;
        } else if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_TRAILING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_CENTER) toAdjust = rectWidth / -2.f;
            else toAdjust = -rectWidth;
        }

        settings.setValue("percentageX", (topleft.x + toAdjust) / MC::windowSize.x);

        settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
        topleft = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    }

    prevAlignments[index] = alignment;

    if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
        if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) topleft.x -= rectWidth;
        else topleft.x -= rectWidth / 2.f;
    }

    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name);

        checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) vec2.x += rectWidth;
            else vec2.x += rectWidth / 2.f;
        }

        topleft.x = vec2.x;
        topleft.y = vec2.y;

        topleft = topleft;

        Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
        settings.setValue("percentageX", percentages.x);
        settings.setValue("percentageY", percentages.y);

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
            if (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING) topleft.x -= rectWidth;
            else topleft.x -= rectWidth / 2.f;
        }
    }

    Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));
    ImVec2 rotationCenter;

    if (rotation > 0.0f) {
        rotationCenter = ImVec2(topleft.x + rectWidth / 2.0f, topleft.y + rectHeight / 2.0f);
        FlarialGUI::ImRotateStart();
    }

    if (getOps<bool>("glow"))
        FlarialGUI::ShadowRect(
            Vec2<float>(topleft.x, topleft.y),
            Vec2<float>(rectWidth, textHeight * getOps<float>("rectheight")),
            getColor("glow"), rounde.x,
            (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));

    float blur = Client::settings.getSettingByName<float>("blurintensity")->value;
    if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(topleft.x, topleft.y, topleft.x + rectWidth, topleft.y + rectHeight), rounde.x, rounde.x));
    //Blur::RenderBlur(SwapchainHook::mainRenderTargetView, 3, blur, topleft.x, topleft.y, rectWidth, rectHeight, rounde.x);


    if (getOps<bool>("rectShadow"))
        FlarialGUI::RoundedRect(
            topleft.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            topleft.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"),
            getColor("rectShadow"),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("showBg"))
        FlarialGUI::RoundedRect(
            topleft.x,
            topleft.y,
            getColor("bg"),
            rectWidth,
            rectHeight,
            rounde.x,
            rounde.x
        );

    if (getOps<bool>("textShadow"))
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("textShadow"),
            true
        );

    if (false) {
        auto txtCol = getColor("text");
        float guiScale = SDK::clientInstance->getGuiData()->getGuiScale();

        SDK::pushDrawTextQueueEntry(DrawTextQueueEntry(
            text,
            RectangleArea(
                (topleft.x + Constraints::SpacingConstraint(paddingX, textWidth)) / guiScale,
                0,
                (topleft.y + Constraints::SpacingConstraint(paddingY, textWidth)) / guiScale,
                0),
            MCCColor(txtCol.r, txtCol.g, txtCol.b, txtCol.a),
            (ui::TextAlignment) alignment,
            TextMeasureData(getOps<float>("textscale") * 2.f, getOps<bool>("textShadow"), false),
            CaretMeasureData{-1, 0}
        ));
    } else {
        FlarialGUI::FlarialTextWithFont(
            topleft.x + Constraints::SpacingConstraint(paddingX, textWidth),
            topleft.y + Constraints::SpacingConstraint(paddingY, textWidth),
            FlarialGUI::to_wide(text).c_str(),
            rectWidth,
            rectHeight,
            alignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("text"),
            true
        );
    }


    if (getOps<bool>("border"))
        FlarialGUI::RoundedHollowRect(
            topleft.x,
            topleft.y,
            Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true),
            getColor("border"),
            rectWidth,
            textHeight * getOps<float>("rectheight"),
            rounde.x,
            rounde.x
        );

    if (rotation > 0.0f) FlarialGUI::ImRotateEnd(rotation + 90.f, rotationCenter);

    if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
}

void Module::normalRender(int index, std::string &value) {
    if (!isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

    std::string text{};
    if (this->settings.getSettingByName<std::string>("text") != nullptr) text = getOps<std::string>("text");

    std::string uppercaseSentence;
    std::string search = "{VALUE}";

    for (char c: text) {
        uppercaseSentence += (char) std::toupper(c);
    }

    size_t pos = uppercaseSentence.find(search);
    if (pos != std::string::npos) {
        text.replace(pos, search.length(), value);
    }

    normalRenderCore(index, text);
}

void Module::resetPadding() {
    padding = 0;
    toggleIndex = 0;
    dropdownIndex = 0;
    sliderIndex = 0;
    buttonIndex = 3;

    int i = 100;
    for (int i = 100; i < colorPickerIndex; ++i) {
        if (color_pickers2.count(i)) FlarialGUI::ColorPickerWindow(i, *color_pickers2[i].value, *color_pickers2[i].opacity, *color_pickers2[i].rgb);
        else FlarialGUI::ColorPickerWindow(i, this->name, color_pickers[i]);
    }

    colorPickerIndex = 100;
    keybindIndex = 0;
    textboxIndex = 300;
    FlarialGUI::UnSetIsInAdditionalYMode();
}

void Module::extraPadding() {
    padding += Constraints::RelativeConstraint(0.04f, "height", true);
}

void Module::addHeader(std::string text) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;


    D2D1_COLOR_F col = ClickGUI::getColor("secondary6");
    col.a = ClickGUI::settingsOpacity;
    D2D1_COLOR_F textCol = clickgui->getColor("headerText");

    std::string name = FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), 500, 0, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::RelativeConstraint(0.215f, "height", true), DWRITE_FONT_WEIGHT_BOLD, textCol, false);

    if (FlarialGUI::shouldAdditionalY)
        for (int i = 0; i < FlarialGUI::highestAddIndexes + 1; i++) {
            if (i <= FlarialGUI::additionalIndex && FlarialGUI::additionalY[i] > 0.0f) {
                y += FlarialGUI::additionalY[i];
            }
        }

    FlarialGUI::RoundedRect(x, y + Constraints::RelativeConstraint(0.023f, "width"), col, FlarialGUI::TextSizes[name] + Constraints::RelativeConstraint(0.01f, "width"), 3.0f, 0, 0);

    padding += Constraints::RelativeConstraint(0.055f, "height", true);
}

void Module::addElementText(std::string text, std::string subtext) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    float subtextY;
    float fontSize = Constraints::RelativeConstraint(0.140f, "height", true);
    float fontSize2 = Constraints::RelativeConstraint(0.12f, "height", true);

    if (!subtext.empty()) {
        y -= Constraints::RelativeConstraint(0.009f, "height", true);
        subtextY = y;
    } else {
        y += Constraints::RelativeConstraint(0.0015f, "height", true);
    }

    D2D1_COLOR_F textCol = clickgui->getColor("settingsText", "ClickGUI");
    D2D1_COLOR_F subtextCol = clickgui->getColor("settingsSubtext", "ClickGUI");
    textCol.a *= clickgui->getOps<float>("_overrideAlphaValues_");
    subtextCol.a *= clickgui->getOps<float>("_overrideAlphaValues_");

    if (ClickGUI::settingsOpacity != 1) {
        textCol.a = ClickGUI::settingsOpacity;
        subtextCol.a = ClickGUI::settingsOpacity;
    }

    FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), 200, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_MEDIUM, textCol, false);

    if (!subtext.empty()) {
        std::vector<std::string> lines;
        std::stringstream ss(subtext);
        std::string line;

        while (std::getline(ss, line, '\n')) lines.push_back(line);

        for (std::string t: lines) {
            subtextY += Constraints::RelativeConstraint(0.017f, "height", true);
            FlarialGUI::FlarialTextWithFont(x, subtextY, FlarialGUI::to_wide(t).c_str(), 200, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize2, DWRITE_FONT_WEIGHT_MEDIUM, subtextCol, false);
        }

        padding += Constraints::RelativeConstraint(0.018f, "height", true) * (lines.size() - 1);
    }
}

void Module::addButton(const std::string &text, const std::string &subtext, const std::string &buttonText, std::function<void()> action) {
    float elementX = Constraints::PercentageConstraint(0.161f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;
    const float width = Constraints::RelativeConstraint(0.09f, "height", true);
    const float height = Constraints::RelativeConstraint(0.035, "height", true);
    Vec2<float> round = Constraints::RoundingConstraint(13, 13);
    D2D1_COLOR_F col = ClickGUI::getColor("primary1");
    col.a = ClickGUI::settingsOpacity;
    D2D1_COLOR_F f = D2D1::ColorF(D2D1::ColorF::White);
    f.a = ClickGUI::settingsOpacity;

    if (FlarialGUI::RoundedButton(buttonIndex, elementX, y - height / 2.f, col, f, FlarialGUI::to_wide(buttonText).c_str(), width, height, round.x, round.y)) {
        action();
    }

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.06f, "height", true);
    buttonIndex++;
}

void Module::addColorPicker(std::string text, std::string subtext, std::string settingName) {
    float elementX = Constraints::PercentageConstraint(0.195f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding; /*

	std::string& value = settings.getSettingByName<std::string>(settingName + "Col")->value;
	float& opac = settings.getSettingByName<float>(settingName + "Opacity")->value;
	bool& rgb = settings.getSettingByName<bool>(settingName + "RGB")->value;*/

    FlarialGUI::ColorPicker(colorPickerIndex, elementX, y, this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);

    color_pickers[colorPickerIndex] = settingName;
    colorPickerIndex++;
}

void Module::addTextBox(std::string text, std::string subtext, std::string &value, int limit) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    FlarialGUI::TextBoxVisual(textboxIndex, value, limit, x, y, "");

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    textboxIndex++;
}

void Module::addTextBox(std::string text, std::string subtext, int limit, std::string settingName) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    std::string &value = getOps<std::string>(settingName);

    FlarialGUI::TextBoxVisual(textboxIndex, value, limit, x, y, "", this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    textboxIndex++;
}

void Module::addDropdown(std::string text, std::string subtext, const std::vector<std::string> &options, std::string settingName, bool resettable) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    Module::addElementText(text, subtext);

    FlarialGUI::Dropdown(dropdownIndex, x, y, options, getOps<std::string>(settingName), "", this->name, settingName);

    if (FlarialGUI::additionalY[dropdownIndex] > 0.f) FlarialGUI::SetIsInAdditionalYMode();

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    dropdownIndex++;
}

void Module::addDropdown(std::string text, std::string subtext, const std::vector<std::string> &options, std::string &value) {
    float x = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    Module::addElementText(text, subtext);


    FlarialGUI::Dropdown(dropdownIndex, x, y, options, value, "");

    if (FlarialGUI::additionalY[dropdownIndex] > 0.f) FlarialGUI::SetIsInAdditionalYMode();

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    dropdownIndex++;
}

void Module::addConditionalTextBox(bool condition, std::string text, std::string subtext, std::string &value, int limit) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalTextBoxAnims[textboxIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalTextBoxAnims[textboxIndex];
        FlarialGUI::lerp(conditionalTextBoxAnims[textboxIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addTextBox(text, subtext, value, limit);
    } else {
        FlarialGUI::lerp(conditionalTextBoxAnims[textboxIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalTextBoxAnims[textboxIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalTextBoxAnims[textboxIndex];
            Module::addTextBox(text, subtext, value, limit);
        } else textboxIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalColorPicker(bool condition, std::string text, std::string subtext, std::string settingName) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalColorPickerAnims[colorPickerIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalColorPickerAnims[colorPickerIndex];
        FlarialGUI::lerp(conditionalColorPickerAnims[colorPickerIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addColorPicker(text, subtext, settingName);
    } else {
        FlarialGUI::lerp(conditionalColorPickerAnims[colorPickerIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalColorPickerAnims[colorPickerIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalColorPickerAnims[colorPickerIndex];
            Module::addColorPicker(text, subtext, settingName);
        } else colorPickerIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addColorPicker(std::string text, std::string subtext, std::string &value, float &opacity, bool &rgb) {
    float elementX = Constraints::PercentageConstraint(0.195f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    FlarialGUI::ColorPicker(colorPickerIndex, elementX, y, value, rgb);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);

    ColorPickerStruct respect = {&value, &opacity, &rgb};
    color_pickers2[colorPickerIndex] = respect;
    colorPickerIndex++;
}

void Module::addConditionalColorPicker(bool condition, std::string text, std::string subtext, std::string &value, float &opacity, bool &rgb) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalColorPickerAnims[colorPickerIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalColorPickerAnims[colorPickerIndex];
        FlarialGUI::lerp(conditionalColorPickerAnims[colorPickerIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addColorPicker(text, subtext, value, opacity, rgb);
    } else {
        FlarialGUI::lerp(conditionalColorPickerAnims[colorPickerIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalColorPickerAnims[colorPickerIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalColorPickerAnims[colorPickerIndex];
            Module::addColorPicker(text, subtext, value, opacity, rgb);
        } else colorPickerIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalDropdown(bool condition, std::string text, std::string subtext, const std::vector<std::string> &options, std::string settingName, bool resettable) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalDropdownAnims[dropdownIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalDropdownAnims[dropdownIndex];
        FlarialGUI::lerp(conditionalDropdownAnims[dropdownIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addDropdown(text, subtext, options, settingName, resettable);
    } else {
        FlarialGUI::lerp(conditionalDropdownAnims[dropdownIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalDropdownAnims[dropdownIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalDropdownAnims[dropdownIndex];
            Module::addDropdown(text, subtext, options, settingName, resettable);
        } else dropdownIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalToggle(bool condition, std::string text, std::string subtext, std::string settingName) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalToggleAnims[toggleIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalToggleAnims[toggleIndex];
        FlarialGUI::lerp(conditionalToggleAnims[toggleIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addToggle(text, subtext, settingName);
    } else {
        FlarialGUI::lerp(conditionalToggleAnims[toggleIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalToggleAnims[toggleIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalToggleAnims[toggleIndex];
            Module::addToggle(text, subtext, settingName);
        } else toggleIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalSlider(bool condition, std::string text, std::string subtext, float &value, float maxVal, float minVal, bool zerosafe) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalSliderAnims[sliderIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalSliderAnims[sliderIndex];
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addSlider(text, subtext, value, maxVal, minVal, zerosafe);
    } else {
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalSliderAnims[sliderIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalSliderAnims[sliderIndex];
            Module::addSlider(text, subtext, value, maxVal, minVal, zerosafe);
        } else sliderIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalSlider(bool condition, std::string text, std::string subtext, std::string settingName, float maxVal, float minVal, bool zerosafe) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalSliderAnims[sliderIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalSliderAnims[sliderIndex];
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addSlider(text, subtext, settingName, maxVal, minVal, zerosafe);
    } else {
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalSliderAnims[sliderIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalSliderAnims[sliderIndex];
            Module::addSlider(text, subtext, settingName, maxVal, minVal, zerosafe);
        } else sliderIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addConditionalSliderInt(bool condition, std::string text, std::string subtext, std::string settingName, int maxVal, int minVal) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalSliderAnims[sliderIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalSliderAnims[sliderIndex];
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addSliderInt(text, subtext, settingName, maxVal, minVal);
    } else {
        FlarialGUI::lerp(conditionalSliderAnims[sliderIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalSliderAnims[sliderIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalSliderAnims[sliderIndex];
            Module::addSliderInt(text, subtext, settingName, maxVal, minVal);
        } else sliderIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addSliderInt(std::string text, std::string subtext, std::string settingName, int maxVal, int minVal) {
    float elementX = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    int &value = getOps<int>(settingName);

    value = std::clamp(value, minVal, maxVal);

    FlarialGUI::SliderInt(sliderIndex, elementX, y, value, maxVal, minVal, this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    sliderIndex++;
}

void Module::addSlider(std::string text, std::string subtext, float &value, float maxVal, float minVal, bool zerosafe) {
    float elementX = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    value = std::clamp(value, minVal, maxVal);

    FlarialGUI::Slider(sliderIndex, elementX, y, value, maxVal, minVal, zerosafe);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    sliderIndex++;
}

void Module::addSlider(std::string text, std::string subtext, std::string settingName, float maxVal, float minVal, bool zerosafe) {
    float elementX = Constraints::PercentageConstraint(0.33f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    float &value = getOps<float>(settingName);

    value = std::clamp(value, minVal, maxVal);

    FlarialGUI::Slider(sliderIndex, elementX, y, value, maxVal, minVal, zerosafe, this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    sliderIndex++;
}

void Module::addConditionalToggle(bool condition, std::string text, std::string subtext, bool &value) {
    FlarialGUI::OverrideAlphaValues((Constraints::RelativeConstraint(0.05f, "height", true) - conditionalToggleAnims[toggleIndex]) / Constraints::RelativeConstraint(0.05f, "height", true));

    if (condition) {
        padding -= conditionalToggleAnims[toggleIndex];
        FlarialGUI::lerp(conditionalToggleAnims[toggleIndex], 0.0f, 0.25f * FlarialGUI::frameFactor);
        Module::addToggle(text, subtext, value);
    } else {
        FlarialGUI::lerp(conditionalToggleAnims[toggleIndex], Constraints::RelativeConstraint(0.05f, "height", true), 0.25f * FlarialGUI::frameFactor);
        if (conditionalToggleAnims[toggleIndex] < Constraints::RelativeConstraint(0.0499f, "height", true)) {
            padding -= conditionalToggleAnims[toggleIndex];
            Module::addToggle(text, subtext, value);
        } else toggleIndex++;
    }

    FlarialGUI::ResetOverrideAlphaValues();
}

void Module::addToggle(std::string text, std::string subtext, std::string settingName) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float elementX = Constraints::PercentageConstraint(0.119f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    FlarialGUI::Toggle(toggleIndex, elementX, y, getOps<bool>(settingName), false, this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    toggleIndex++;
}

void Module::addToggle(std::string text, std::string subtext, bool &value) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float elementX = Constraints::PercentageConstraint(0.119f, "right");
    float y = Constraints::PercentageConstraint(0.10, "top") + padding;

    if (FlarialGUI::Toggle(toggleIndex, elementX, y, value, false)) value = !value;

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    toggleIndex++;
}

void Module::addKeybind(std::string text, std::string subtext, std::string &keybind) {
    float elementX = Constraints::PercentageConstraint(0.134f, "right");
    float y = Constraints::PercentageConstraint(0.08, "top") + padding;

    FlarialGUI::KeybindSelector(keybindIndex, elementX, y, keybind);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    keybindIndex++;
}

void Module::addKeybind(std::string text, std::string subtext, std::string settingName, bool resettable) {
    float elementX = Constraints::PercentageConstraint(0.134f, "right");
    float y = Constraints::PercentageConstraint(0.08, "top") + padding;

    FlarialGUI::KeybindSelector(keybindIndex, elementX, y, getOps<std::string>(settingName), this->name, settingName);

    Module::addElementText(text, subtext);

    padding += Constraints::RelativeConstraint(0.05f, "height", true);
    keybindIndex++;
}

void Module::postLoad(bool softLoad) {
    this->totalKeybinds = 0;
    this->totalWaypoints = 0;
    this->totalmaps = 0;
    keybindActions.clear();

    if (Client::hasLegacySettings) this->loadLegacySettings();

    if (!isScripting()) this->defaultConfig();
    else this->defaultConfig();

    keybindActions.push_back([this](std::vector<std::any> args)-> std::any {
        if (SDK::getCurrentScreen() != "hud_screen" &&
            SDK::getCurrentScreen() != "zoom_screen" &&
            SDK::getCurrentScreen() != "f3_screen" && this->name != "ClickGUI"
        )
            return {};
        this->active = !this->active;
        return {};
    });

    if (!softLoad && this->settings.getSettingByName<bool>("enabled") && getOps<bool>("enabled")) {
        this->onEnable();
        this->enabledState = true;
    }

    for (const auto &settingPair: settings.settings) {
        const std::string &name = settingPair.first;
        if (name.find("keybind") != std::string::npos) {
            ++totalKeybinds;
        } else if (name.find("waypoint") != std::string::npos) {
            ++totalWaypoints;
        } else if (name.find("map-") != std::string::npos) {
            ++totalmaps;
        }
    }

    this->onSetup();
}

void Module::loadLegacySettings() {
    if (isScripting() || !Client::legacySettings.getSettingByName<std::string>("currentConfig")) return;
    if (Client::legacySettings.getSettingByName<std::string>("currentConfig")->value != "default") legacySettingsPath = std::filesystem::path(Client::legacyDir) / Client::legacySettings.getSettingByName<std::string>("currentConfig")->value / (name + ".flarial");
    else if (std::filesystem::exists(Client::legacyDir + "\\default") && std::filesystem::is_directory(Client::legacyDir + "\\default")) legacySettingsPath = std::filesystem::path(Client::legacyDir) / "default" / (name + ".flarial");
    else legacySettingsPath = std::filesystem::path(Client::legacyDir) / (name + ".flarial");

    if (!std::filesystem::exists(legacySettingsPath)) return;

    std::ifstream inputFile(legacySettingsPath);
    if (!inputFile.is_open()) {
        Logger::error("Failed to open legacy settings file: {}", legacySettingsPath.string());
        return;
    }

    std::stringstream ss;
    ss << inputFile.rdbuf();
    inputFile.close();

    if (!ss.str().empty() && ss.str() != "null") this->settings.AppendFromJson(ss.str(), true);
}

void Module::loadSettings(bool softLoad) {
    if (this->isScripting()) {
        if (!std::filesystem::exists(settingspath)) {
            std::filesystem::create_directories(settingspath.parent_path());
            std::ofstream outputFile(settingspath);
            if (!outputFile.is_open()) {
                LOG_ERROR("Failed to create file: {}", settingspath.string());
            }
            outputFile.close();
        }
        std::ifstream inputFile(this->settingspath);
        if (!inputFile.is_open()) {
            LOG_ERROR("Failed to open file: {}", this->settingspath.string());
            return;
        }

        std::stringstream ss;
        ss << inputFile.rdbuf();
        inputFile.close();

        if (!ss.str().empty() && ss.str() != "null") this->settings.FromJson(ss.str());
        else this->defaultConfig("all");
    } else {
        try { settings.FromJson(Client::globalSettings[name].dump()); } catch (std::exception &e) { Logger::error("Couldn't load module settings: {}", e.what()); }
    }


    this->postLoad(softLoad);
}

void Module::toggle() {
    enabledState = !enabledState;
}

void Module::setup() {
    if (!isScripting()) this->defaultConfig();
    else this->defaultConfig();

    keybindActions.push_back([this](std::vector<std::any> args)-> std::any {
        this->active = !this->active;
        return {};
    });

    onSetup();
}

void Module::onSetup() {
}

// TODO: rename to Enable/Disable?
void Module::onEnable() {
    Telemetry::sendModuleEvent(this->name, "enable");

    enabledState = true;
    if (settings.getSettingByName<bool>("enabled")) getOps<bool>("enabled") = true;
    // Client::SaveSettings();
}

void Module::onDisable() {
    Telemetry::sendModuleEvent(this->name, "disable");

    enabledState = false;
    active = false;
    if (!terminating) {
        if (settings.getSettingByName<bool>("enabled")) getOps<bool>("enabled") = false;
    }
    // Client::SaveSettings();
}

void Module::terminate() {
    terminating = true;
    onDisable();
    active = false;
}

// TODO: find all getSettingByName<bool>("enabled")->value and replace!!!
bool Module::isEnabled() {
    if (isScripting() && !ScriptManager::initialized) return false;
    if (!settings.getSettingByName<bool>("enabled")) return false;
    return getOps<bool>("enabled");
}

void Module::setEnabled(bool enabled) {
    if (!settings.getSettingByName<bool>("enabled")) return;

    getOps<bool>("enabled") = enabled;
    enabledState = enabled;
}

void Module::setKeybind(const std::string &newKeybind) {
    auto key = settings.getSettingByName<std::string>("keybind");
    if (key == nullptr) settings.addSetting("keybind", newKeybind);
}

std::string &Module::getKeybind(const int keybindCount) {
    std::string count;
    if (keybindCount > 0) count = "-" + FlarialGUI::cached_to_string(keybindCount);
    auto key = settings.getSettingByName<std::string>("keybind" + count);
    if (key == nullptr) settings.addSetting("keybind", defaultKeybind);
    return key->value;
}

std::string &Module::getKeybind(const int keybindCount, bool whoCaresIfItsZeroOrNotTf) {
    std::string count = "-" + FlarialGUI::cached_to_string(keybindCount);
    auto key = settings.getSettingByName<std::string>("keybind" + count);
    if (key == nullptr) settings.addSetting("keybind", defaultKeybind);
    return key->value;
}

void Module::defaultAddSettings(std::string type) {
    if (type == "main") {
        addSlider("UI Scale", "", "uiscale", 2.0f);
        addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", "BlurEffect");
        addSlider("Rounding", "Rounding of the rectangle", "rounding");
        addToggle("Background", "", "showBg");
        addConditionalToggle(getOps<bool>("showBg"), "Background Shadow", "Displays a shadow under the background", "rectShadow");
        addConditionalSlider(getOps<bool>("showBg") && getOps<bool>("rectShadow"), "Shadow Offset", "How far the shadow will be.", "rectShadowOffset", 0.02f, 0.001f);
        addToggle("Border", "", "border");
        addConditionalSlider(getOps<bool>("border"), "Border Thickness", "", "borderWidth", 4.f);
        addToggle("Glow", "", "glow");
        addConditionalSlider(getOps<bool>("glow"), "Glow Amount", "", "glowAmount", 100.f);
    } else if (type == "text") {
        addTextBox("Format", "", 0, "text");
        addSlider("Text Scale", "", "textscale", 2.0f);
        addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment", true);
        addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
        addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    } else if (type == "colors") {
        addColorPicker("Text Color", "", "text");
        addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
        addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
        addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
        addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
        addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");
    } else if (type == "misc") {
        addToggle("Responsive Rectangle", "Rectangle resizes with text", "responsivewidth");
        addToggle("Reverse Padding X", "For Text Position", "reversepaddingx");
        addToggle("Reverse Padding Y", "For Text Position", "reversepaddingy");
        addSlider("Padding X", "For Text Position", "padx", 1.f);
        addSlider("Padding Y", "For Text Position", "pady", 1.f);
        addSlider("Rectangle Width", "", "rectwidth", 2.f, 0.001f);
        addSlider("Rectangle Height", "", "rectheight", 2.f, 0.001f);
        addSlider("Rotation", "", "rotation", 360.f, 0, false);
    }
}

void Module::defaultConfig(std::string type) {
    settings.renameSetting("bgColor", "bgCol");
    settings.renameSetting("textColor", "textCol");
    settings.renameSetting("glowColor", "glowCol");
    settings.renameSetting("borderColor", "borderCol");
    if (type == "core") {
        setDef("enabled", false);
        setDef("favorite", false);
    }
    if (type == "pos") {
        setDef("percentageX", 0.0f);
        setDef("percentageY", 0.0f);
    } else if (type == "main") {
        setDef("uiscale", 0.65f);
        setDef("BlurEffect", false);
        setDef("rounding", 32.0f);
        setDef("showBg", true);
        setDef("rectShadow", false);
        setDef("rectShadowOffset", 0.003f);
        setDef("border", false);
        setDef("borderWidth", 1.0f);
        setDef("glow", false);
        setDef("glowAmount", 30.0f);
    } else if (type == "text") {
        setDef("text", (std::string) "{value}");
        setDef("textscale", 1.0f);
        setDef("textalignment", (std::string) "Center");
        setDef("textShadow", false);
        setDef("textShadowOffset", 0.003f);
    } else if (type == "colors") {
        setDef("text", (std::string) "fafafa", 1.f, false);
        setDef("bg", (std::string) "000000", 0.55f, false);
        setDef("rectShadow", (std::string) "00000", 0.55f, false);
        setDef("textShadow", (std::string) "00000", 0.55f, false);
        setDef("border", (std::string) "000000", 1.f, false);
        setDef("glow", (std::string) "F0F0F0", 1.f, false);
    } else if (type == "misc") {
        setDef("responsivewidth", true);
        setDef("reversepaddingx", false);
        setDef("reversepaddingy", false);
        setDef("padx", 0.0f);
        setDef("pady", 0.0f);
        setDef("rectwidth", 1.0f);
        setDef("rectheight", 1.0f);
        setDef("rotation", 0.0f);
    } else if (type == "all") {
        defaultConfig("core");
        defaultConfig("pos");
        defaultConfig("main");
        defaultConfig("text");
        defaultConfig("colors");
        defaultConfig("misc");
    }
}

void Module::defaultConfig() {
    getKeybind();
    defaultConfig("core");
}

bool Module::isKeybind(const std::array<bool, 256> &keys, const int keybindCount) {
    getKeybind();
    std::string count = "keybind";
    if (keybindCount > 0) count += "-" + FlarialGUI::cached_to_string(keybindCount);
    if (!settings.getSettingByName<std::string>(count)) { return false; }

    std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind(keybindCount));

    for (int keyCode: keyCodes) {
        if (!keys[keyCode]) {
            return false;
        }
    }
    // TODO: in menu should also mean in Edit Mode, when edit mode all modules set to active(visible? add visible setting (for TAB))
    if (name != "ClickGUI") if (FlarialGUI::inMenu) return false;
    if (SDK::getCurrentScreen() == "chat_screen") return false;
    bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const auto &pair) {
        return !pair.second.isActive;
    });
    return allInactive;
}

[[nodiscard]] bool Module::isAdditionalKeybind(const std::array<bool, 256> &keys, const std::string &bind) const {
    std::vector<int> keyCodes = Utils::getStringAsKeys(bind);

    for (int keyCode: keyCodes) {
        if (!keys[keyCode]) {
            return false;
        }
    }
    if (name != "ClickGUI") if (FlarialGUI::inMenu) return false;
    if (SDK::getCurrentScreen() == "chat_screen") return false;
    bool allInactive = std::ranges::all_of(FlarialGUI::TextBoxes, [](const auto &pair) {
        return !pair.second.isActive; // Access the value in the map
    });
    // All keys in the keybind are being held down
    return allInactive;
}

bool Module::isKeyPartOfKeybind(int keyCode, const int keybindCount) {
    std::vector<int> keyCodes = Utils::getStringAsKeys(getKeybind(keybindCount));
    return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
}

bool Module::isKeyPartOfAdditionalKeybind(int keyCode, const std::string &bind) {
    std::vector<int> keyCodes = Utils::getStringAsKeys(bind);
    return std::find(keyCodes.begin(), keyCodes.end(), keyCode) != keyCodes.end();
}

long long _lastScrollId = 0;

void Module::checkForRightClickAndOpenSettings(float x, float y, float width, float height) {
    if (MC::scrollId != _lastScrollId) {
        if (FlarialGUI::CursorInRect(x, y, width, height)) {
            if (MC::lastMouseScroll == MouseAction::ScrollUp) {
                auto uiscale = settings.getSettingByName<float>("uiscale");
                if (uiscale != nullptr) uiscale->value = std::min(5.f, uiscale->value + 0.05f);
                _lastScrollId = MC::scrollId;
            } else {
                auto uiscale = settings.getSettingByName<float>("uiscale");
                if (uiscale != nullptr) uiscale->value = std::max(0.01f, uiscale->value - 0.05f);
                _lastScrollId = MC::scrollId;
            }
        }
    }
    if (FlarialGUI::CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Right && MC::held) {
        auto module = ModuleManager::getModule("ClickGUI");
        if (module != nullptr) {
            module->active = true;
            ClickGUI::editmenu = false;
            FlarialGUI::TextBoxes[0].isActive = false;
            ClickGUI::page.type = "settings";
            ClickGUI::page.module = this->name;
            ClickGUI::curr = "settings";
            FlarialGUI::scrollpos = 0;
            FlarialGUI::barscrollpos = 0;
            if (Client::settings.getSettingByName<bool>("saveScrollPos")->value) {
                ClickGUI::saved_acumilatedPos = ClickGUI::accumilatedPos;
                ClickGUI::saved_acumilatedBarPos = ClickGUI::accumilatedBarPos;
            }
            ClickGUI::accumilatedPos = 0;
            ClickGUI::accumilatedBarPos = 0;
        }
    }
}
