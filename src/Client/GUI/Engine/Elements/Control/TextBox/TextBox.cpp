#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_primary1 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary1")->value)
#define o_colors_primary1 clickgui->settings.getSettingByName<float>("o_colors_primary1")->value
#define colors_primary1_rgb clickgui->settings.getSettingByName<bool>("colors_primary1_rgb")->value

#define colors_primary2 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary2")->value)
#define o_colors_primary2 clickgui->settings.getSettingByName<float>("o_colors_primary2")->value
#define colors_primary2_rgb clickgui->settings.getSettingByName<bool>("colors_primary2_rgb")->value

#define colors_primary3 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary3")->value)
#define o_colors_primary3 clickgui->settings.getSettingByName<float>("o_colors_primary3")->value
#define colors_primary3_rgb clickgui->settings.getSettingByName<bool>("colors_primary3_rgb")->value

#define colors_primary4 HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_primary4")->value)
#define o_colors_primary4 clickgui->settings.getSettingByName<float>("o_colors_primary4")->value
#define colors_primary4_rgb clickgui->settings.getSettingByName<bool>("colors_primary4_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

//TODO: this defo laggs req investigation
static std::string rReal;
static std::string ttext;

std::string FlarialGUI::TextBoxVisual(int index, std::string &text, int limit, float x, float y, const std::string& real) {
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }

    if (FlarialGUI::TextBoxes[index].isActive) {
        if (FlarialGUI::TextBoxes[index].isAt1) FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
    } else FlarialGUI::TextBoxes[index].cursorOpac = 0;

    if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
    if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

    D2D1_COLOR_F col;

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    const bool isAdditionalY = shouldAdditionalY;
    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

    y -= percHeight / 2.0f;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    text = FlarialGUI::TextBox(index, text, limit, x, y, Constraints::SpacingConstraint(1.55, textWidth), percHeight);

    if (TextBoxes[index].isActive) {
        col = colors_primary1;
        col.a = o_colors_primary1;
    } else {
        col = colors_primary3;
        col.a = o_colors_primary3;
    }

    col.a = ClickGUI::settingsOpacity;


    FlarialGUI::RoundedRect(x, y, col, Constraints::SpacingConstraint(1.55, textWidth), percHeight, round.x, round.x);

    float textSize = Constraints::SpacingConstraint(1.0, textWidth);

    ttext = FlarialGUI::FlarialTextWithFont(x + Constraints::RelativeConstraint(0.05f), y, FlarialGUI::to_wide(text).c_str(),
                                    Constraints::SpacingConstraint(1.55, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, textSize, DWRITE_FONT_WEIGHT_NORMAL);

    D2D1_COLOR_F cursorCol = colors_primary2_rgb ? rgbColor : colors_primary2;
    cursorCol.a = o_colors_primary2;

    cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

    FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
                     x + FlarialGUI::TextSizes[ttext] + Constraints::RelativeConstraint(0.055f),
                     0.420f * FlarialGUI::frameFactor);

    if (FlarialGUI::TextBoxes[index].cursorX > x)
        FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX,
                                y + Constraints::RelativeConstraint(0.045f) / 2.0f, cursorCol,
                                Constraints::RelativeConstraint(0.005f),
                                percHeight - Constraints::RelativeConstraint(0.045f), 0, 0);

    rReal = FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.70, textWidth), y,
                                    FlarialGUI::to_wide(real).c_str(),
                                    Constraints::SpacingConstraint(6.9, textWidth), percHeight,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.00, textWidth),
                                    DWRITE_FONT_WEIGHT_NORMAL);

    if (isAdditionalY) SetIsInAdditionalYMode();

    return "";
}

std::string FlarialGUI::TextBox(int index, const std::string& text, int limit, float x, float y, float width, float height) {
    if (isInScrollView) y += scrollpos;

    if (CursorInRect(x, y, width, height) && MC::mouseAction == MouseAction::Press &&
        MC::mouseButton == MouseButton::Left && !activeColorPickerWindows) {

        FlarialGUI::TextBoxes[index].isActive = true;

    } else if (!CursorInRect(x, y, width, height) && MC::mouseAction == MouseAction::Press &&
               MC::mouseButton == MouseButton::Left) {

        FlarialGUI::TextBoxes[index].isActive = false;
        FlarialGUI::TextBoxes[index].text = text;

    } else if (!FlarialGUI::TextBoxes[index].isActive && !activeColorPickerWindows) {

        FlarialGUI::TextBoxes[index].text = text;
    }


    if (FlarialGUI::TextBoxes[index].text.empty() && FlarialGUI::TextBoxes[index].firstTime) {
        FlarialGUI::TextBoxes[index].firstTime = false;
        FlarialGUI::TextBoxes[index].text = text;
    }

    FlarialGUI::TextBoxes[index].text = FlarialGUI::TextBoxes[index].text.substr(0, limit);

    return FlarialGUI::TextBoxes[index].text;
}