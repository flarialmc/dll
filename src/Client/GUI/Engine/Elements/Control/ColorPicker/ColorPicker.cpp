#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

using namespace winrt::Windows::UI::Core;
static std::map<std::string, std::string> sizes;

void FlarialGUI::ColorPicker(const int index, float x, float y, std::string moduleName, std::string settingName) {
    auto mod = ModuleManager::getModule(moduleName);
    // Accepts hex, so for e.g. fps counter bg color wants to be changed then you'd have to give a modifyable hex value
    // Preferably save every color in config as a hex (string)
    // before rendering just convert the config's color to hex and yeah do it dat way.
    // the Hex parameter in this function should be a pointer directly to the .value of the config
    // change the loadSettings, see if the config name contains "color" and see if the value is 6 characters long, if so,
    // then load the setting as uint32_t instead of string
    // but when saving, it should be converted to string.

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

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true); // this is the height of whole rect;

    y -= s / 2.f;

    D2D1_COLOR_F baseColor = ClickGUI::getColor("primary4");
    baseColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

    FlarialGUI::RoundedRect(x, y + s * 0.15f, baseColor, s * 4.125f, s, round.x, round.x);

    round = Constraints::RoundingConstraint(10, 10);

    if (mod->getOps<bool>(settingName + "RGB")) {
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
        D2D1_COLOR_F color = mod->getColor(settingName);
        color.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
        FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, color, s * 0.85f, s * 0.85f, round.x, round.x);
    }

    round = Constraints::RoundingConstraint(11.5, 11.5);

    if (TextBoxes[index].noCursorBgCol) {
        TextBoxes[index].curBgCol = ClickGUI::getColor("primary3");
        TextBoxes[index].noCursorBgCol = false;
    }

    D2D1_COLOR_F col = ClickGUI::getColor("primary3");
    if (TextBoxes[index].isActive) col = ClickGUI::getColor("primary1");

    TextBoxes[index].curBgCol = FlarialGUI::LerpColor(TextBoxes[index].curBgCol, CursorInRect(x, y + (isInScrollView ? scrollpos : 0), s * 3.f, s * 0.82f) ? D2D1::ColorF(col.r * 0.8, col.g * 0.8, col.b * 0.8, col.a) : col, 0.1f * FlarialGUI::frameFactor);

    col.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

    if (CursorInRect(x + Constraints::SpacingConstraint(1.05, s), y + (isInScrollView ? scrollpos : 0), s * 3.f, s * 0.82f) && !TextBoxes[index].isHovering) {
        TextBoxes[index].isHovering = true;
        WinrtUtils::setCursorTypeThreaded(CoreCursorType::IBeam);
    } else if (!CursorInRect(x + Constraints::SpacingConstraint(1.05, s), y + (isInScrollView ? scrollpos : 0), s * 3.f, s * 0.82f) && TextBoxes[index].isHovering) {
        TextBoxes[index].isHovering = false;
        WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
    }

    if (CursorInRect(x, y + (isInScrollView ? scrollpos : 0) + s * 0.21f, s * 0.85f, s * 0.85f) && !ColorPickers[index].isHovering) {
        ColorPickers[index].isHovering = true;
        WinrtUtils::setCursorTypeThreaded(CoreCursorType::Hand);
    } else if (!CursorInRect(x, y + (isInScrollView ? scrollpos : 0) + s * 0.21f, s * 0.85f, s * 0.85f) && ColorPickers[index].isHovering) {
        ColorPickers[index].isHovering = false;
        WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
    }

    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, TextBoxes[index].curBgCol, s * 3.f, s * 0.82f, round.x, round.x);

    std::string text;
    mod->getOps<std::string>(settingName + "Col") = FlarialGUI::TextBox(index, mod->getOps<std::string>(settingName + "Col"), 6, x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, s * 3.f,
                                                                        s * 0.82f, 1);

    text = "#" + mod->getOps<std::string>(settingName + "Col");
    //textLayout->Release();

    D2D1_COLOR_F cursorCol = ClickGUI::getColor("primary2");

    cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y -= additionalY[i];
            }
        }
    }

    sizes[text] = FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.35f, s), y * 1.0025f,
                                                  FlarialGUI::to_wide(text).c_str(), s * 4.3f, s * 1.1f,
                                                  DWRITE_TEXT_ALIGNMENT_LEADING, s * 4.0f, DWRITE_FONT_WEIGHT_NORMAL);

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }

    FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
                     x + Constraints::SpacingConstraint(0.555f, s) + TextSizes[sizes[text]] +
                     Constraints::RelativeConstraint(0.055f), 0.420f * FlarialGUI::frameFactor);

    if (FlarialGUI::TextBoxes[index].cursorX > x)
        FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX, y + Constraints::RelativeConstraint(0.05f) / 2.0f,
                                cursorCol, Constraints::RelativeConstraint(0.005f),
                                s * 0.82f - Constraints::RelativeConstraint(0.025f), 0, 0);

    float clickingY = y;
    if (isInScrollView) clickingY += scrollpos;

    if (CursorInRect(x + Constraints::SpacingConstraint(0.1, s), clickingY + s * 0.21f, s * 0.85f, s * 0.85f) && MC::mouseButton == MouseButton::Right && !MC::held) {
        bool resettableSettingsEnabled = Client::settings.getSettingByName<bool>("resettableSettings")->value;
        if (resettableSettingsEnabled && moduleName != "" && settingName != "") {
            activeColorPickerWindows = 0;
            mod->settings.deleteSetting(settingName + "Col");
            mod->settings.deleteSetting(settingName + "Opacity");
            mod->settings.deleteSetting(settingName + "RGB");
            resetColorPicker(index);
            mod->defaultConfig();
            text = "#" + mod->getOps<std::string>(settingName + "Col");
        }
    }

    if (!activeColorPickerWindows &&
        CursorInRect(x + Constraints::SpacingConstraint(0.1, s), clickingY + s * 0.21f, s * 0.85f, s * 0.85f) &&
        MC::mouseButton == MouseButton::Left && MC::held) {
        MC::mouseButton = MouseButton::None;
        ColorPickers[index].isActive = true;
        FlarialGUI::ColorPickerWindow(index, moduleName, settingName);
        activeColorPickerWindows++;
    }
}

void FlarialGUI::ColorPicker(const int index, float x, float y, std::string &hex, bool &rgb, std::string moduleName, std::string settingName) {
    // Accepts hex, so for e.g. fps counter bg color wants to be changed then you'd have to give a modifyable hex value
    // Preferably save every color in config as a hex (string)
    // before rendering just convert the config's color to hex and yeah do it dat way.
    // the Hex parameter in this function should be a pointer directly to the .value of the config
    // change the loadSettings, see if the config name contains "color" and see if the value is 6 characters long, if so,
    // then load the setting as uint32_t instead of string
    // but when saving, it should be converted to string.

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

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true); // this is the height of whole rect;

    y -= s / 2.f;

    D2D1_COLOR_F baseColor = ClickGUI::getColor("primary4");

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
        color.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
        FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, color, s * 0.85f, s * 0.85f, round.x, round.x);
    }

    round = Constraints::RoundingConstraint(11.5, 11.5);

    D2D1_COLOR_F col = ClickGUI::getColor("primary3");

    if (TextBoxes[index].isActive) col = ClickGUI::getColor("primary1");

    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, col, s * 3.f, s * 0.82f,
                            round.x, round.x);

    std::string text;
    hex = FlarialGUI::TextBox(index, hex, 6, x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, s * 3.f,
                              s * 0.82f, 1);

    text = "#" + hex;
    //textLayout->Release();

    D2D1_COLOR_F cursorCol = ClickGUI::getColor("primary2");

    cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y -= additionalY[i];
            }
        }
    }

    sizes[text] = FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.35f, s), y * 1.0025f,
                                                  FlarialGUI::to_wide(text).c_str(), s * 4.3f, s * 1.1f,
                                                  DWRITE_TEXT_ALIGNMENT_LEADING, s * 4.0f, DWRITE_FONT_WEIGHT_NORMAL);

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }

    FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX,
                     x + Constraints::SpacingConstraint(0.555f, s) + TextSizes[sizes[text]] +
                     Constraints::RelativeConstraint(0.055f), 0.420f * FlarialGUI::frameFactor);

    if (FlarialGUI::TextBoxes[index].cursorX > x)
        FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX, y + Constraints::RelativeConstraint(0.05f) / 2.0f,
                                cursorCol, Constraints::RelativeConstraint(0.005f),
                                s * 0.82f - Constraints::RelativeConstraint(0.025f), 0, 0);

    float clickingY = y;
    if (isInScrollView) clickingY += scrollpos;

    if (CursorInRect(x + Constraints::SpacingConstraint(0.1, s), clickingY + s * 0.21f, s * 0.85f, s * 0.85f) && MC::mouseButton == MouseButton::Right && !MC::held) {
        bool resettableSettingsEnabled = Client::settings.getSettingByName<bool>("resettableSettings")->value;
        if (resettableSettingsEnabled && moduleName != "" && settingName != "") {
            auto mod = ModuleManager::getModule(moduleName);
            mod->settings.deleteSetting(settingName);
            mod->defaultConfig();
            hex = mod->settings.getSettingByName<std::string>(settingName)->value;
            text = "#" + hex;
        }
    }

    if (!activeColorPickerWindows &&
        CursorInRect(x + Constraints::SpacingConstraint(0.1, s), clickingY + s * 0.21f, s * 0.85f, s * 0.85f) &&
        MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        ColorPickers[index].isActive = true;
        activeColorPickerWindows++;
    }
}
