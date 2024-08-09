#include "../../../Engine.hpp"
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

static std::map<std::string, std::string> sizes;

void FlarialGUI::ColorPicker(const int index, float x, float y, std::string &hex, bool &rgb) {

    // Accepts hex, so for e.g. fps counter bg color wants to be changed then you'd have to give a modifyable hex value
    // Preferably save every color in config as a hex (string)
    // before rendering just convert the config's color to hex and yeah do it dat way.
    // the Hex parameter in this function should be a pointer directly to the .value of the config
    // change the loadSettings, see if the config name contains "color" and see if the value is 6 characters long, if so,
    // then load the setting as uint32_t instead of string
    // but when saving, it should be converted to string.

    if (FlarialGUI::TextBoxes[index].isActive) {
        if (FlarialGUI::TextBoxes[index].isAt1)
            FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
    } else FlarialGUI::TextBoxes[index].cursorOpac = 0;

    if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
    if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true); // this is the height of whole rect;

    y -= s / 2.f;

    D2D1_COLOR_F baseColor = colors_primary4_rgb ? rgbColor : colors_primary4;
    baseColor.a = o_colors_primary4;

    FlarialGUI::RoundedRect(x, y + s * 0.15f, baseColor, s * 4.125f, s, round.x, round.x);

    round = Constraints::RoundingConstraint(10, 10);

    if (rgb) {
        FlarialGUI::image(
                IDR_RGB_PNG,
                D2D1::RectF(
                        x + Constraints::SpacingConstraint(0.1, s),
                        y + s * 0.21f,
                        x + Constraints::SpacingConstraint(0.1, s) + s * 0.85f,
                        y + s * 0.21f + s * 0.85f
                )
        );
    } else {
        D2D1_COLOR_F color = FlarialGUI::HexToColorF(hex);
        FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, color, s * 0.85f, s * 0.85f,
                                round.x, round.x);
    }

    round = Constraints::RoundingConstraint(11.5, 11.5);

    D2D1_COLOR_F col = colors_primary3_rgb ? rgbColor : colors_primary3;

    if (TextBoxes[index].isActive) col = colors_primary1_rgb ? rgbColor : colors_primary1;

    col.a = TextBoxes[index].isActive ? o_colors_primary1 : o_colors_primary3;

    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, col, s * 3.f, s * 0.82f,
                            round.x, round.x);

    std::string text;
    hex = FlarialGUI::TextBox(index, hex, 6, x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, s * 3.f,
                              s * 0.82f);

    text = "#" + hex;
    //textLayout->Release();

    D2D1_COLOR_F cursorCol = colors_primary2_rgb ? rgbColor : colors_primary2;
    cursorCol.a = o_colors_primary2;

    cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;


    sizes[text] = FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.35f, s), y * 1.006f,
                                    FlarialGUI::to_wide(text).c_str(), s * 4.3f, s * 1.1f,
                                    DWRITE_TEXT_ALIGNMENT_LEADING, s * 4.0f, DWRITE_FONT_WEIGHT_NORMAL);

    FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
                 x + Constraints::SpacingConstraint(0.555f, s) + TextSizes[sizes[text]] +
                 Constraints::RelativeConstraint(0.055f), 0.420f * FlarialGUI::frameFactor);

    if (FlarialGUI::TextBoxes[index].cursorX > x)
        FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX, y + Constraints::RelativeConstraint(0.05f) / 2.0f,
                                cursorCol, Constraints::RelativeConstraint(0.005f),
                                s * 0.82f - Constraints::RelativeConstraint(0.025f), 0, 0);

    float clickingY = y;
    if (isInScrollView) clickingY += scrollpos;

    if (!activeColorPickerWindows &&
        CursorInRect(x + Constraints::SpacingConstraint(0.1, s), clickingY + s * 0.21f, s * 0.85f, s * 0.85f) &&
        MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        ColorPickers[index].isActive = true;
        activeColorPickerWindows++;
    }

}