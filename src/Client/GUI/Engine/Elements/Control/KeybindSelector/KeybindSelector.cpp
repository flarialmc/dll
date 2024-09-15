#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../../../../Module/Manager.hpp"

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

void FlarialGUI::KeybindSelector(const int index, float x, float y, std::string &keybind) {

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (FlarialGUI::DropDownMenus[i].isActive && i <= additionalIndex) {
                y += additionalY[i];
            }
        }
    }

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);
    const bool isAdditionalY = shouldAdditionalY;
    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percWidth = Constraints::RelativeConstraint(0.069, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

    if (!KeybindSelectors[index].curColorDone) {
        KeybindSelectors[index].curColor = D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f);
        KeybindSelectors[index].curColorDone = true;
    }

    D2D1_COLOR_F col = KeybindSelectors[index].isActive ? (colors_primary1_rgb ? rgbColor : colors_primary1)
                                                        : (colors_primary3_rgb ? rgbColor : colors_primary3);
    col.a = KeybindSelectors[index].isActive ? o_colors_primary1 : o_colors_primary3;

    if (KeybindSelectors[index].isActive) {
        std::chrono::steady_clock::time_point currentOnKeyTime = std::chrono::steady_clock::now();
        auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentOnKeyTime - KeybindSelectors[index].currentOnKeyTime);

        if (timeDifference.count() > 2000) KeybindSelectors[index].isActive = false;
    }

    KeybindSelectors[index].curColor = FlarialGUI::LerpColor(KeybindSelectors[index].curColor, col,
                                                             0.1f * FlarialGUI::frameFactor);

    std::string text;

    if (KeybindSelectors[index].isActive) {
        if (FlarialGUI::currentKeybind != "nothing") {

            KeybindSelectors[index].oldShi = keybind;
            keybind = FlarialGUI::currentKeybind;
            KeybindSelectors[index].newShi = keybind;

        } else {
            FlarialGUI::currentKeybind = "";
            keybind = "";
        }
    }

    text = keybind;

    if (isAdditionalY) UnSetIsInAdditionalYMode();
    FlarialGUI::RoundedRect(x, y, KeybindSelectors[index].curColor, percWidth, percHeight, round.x, round.x);

    if(text != "" & text != " ")
    FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), percWidth, percHeight,
                                    DWRITE_TEXT_ALIGNMENT_CENTER, textWidth, DWRITE_FONT_WEIGHT_NORMAL);
    else  FlarialGUI::FlarialTextWithFont(x, y, L"unset", percWidth, percHeight,
                                    DWRITE_TEXT_ALIGNMENT_CENTER, textWidth, DWRITE_FONT_WEIGHT_NORMAL);


    if (isAdditionalY) SetIsInAdditionalYMode();

    if (isInScrollView) y += scrollpos;

    if (!activeColorPickerWindows && CursorInRect(x, y, percWidth, percHeight) &&
        MC::mouseButton == MouseButton::Left && !MC::held && !KeybindSelectors[index].isActive) {
        KeybindSelectors[index].isActive = true;
        KeybindSelectors[index].currentOnKeyTime = std::chrono::steady_clock::now();
        MC::mouseButton = MouseButton::None;
    }

    if (!CursorInRect(x, y, percWidth, percHeight) && MC::mouseButton == MouseButton::Left && !MC::held) {
        KeybindSelectors[index].isActive = false;

    }

}