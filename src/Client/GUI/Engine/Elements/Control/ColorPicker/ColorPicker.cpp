#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

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
        if (FlarialGUI::TextBoxes[index].isAt1)
            FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
        else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
    } else FlarialGUI::TextBoxes[index].cursorOpac = 0;

    if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
    if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true); // this is the height of whole rect;

    y -= s / 2.f;

    D2D1_COLOR_F baseColor = clickgui->getColor("primary4", "ClickGUI");

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
        color.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
        FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, color, s * 0.85f, s * 0.85f, round.x, round.x);
    }

    round = Constraints::RoundingConstraint(11.5, 11.5);

    D2D1_COLOR_F col = clickgui->getColor("primary3", "ClickGUI");

    if (TextBoxes[index].isActive) col = clickgui->getColor("primary1", "ClickGUI");

    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, col, s * 3.f, s * 0.82f,
                            round.x, round.x);

    std::string text;
    mod->getOps<std::string>(settingName + "Col") = FlarialGUI::TextBox(index, mod->getOps<std::string>(settingName + "Col"), 6, x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, s * 3.f,
                              s * 0.82f, 1);

    text = "#" + mod->getOps<std::string>(settingName + "Col");
    //textLayout->Release();

    D2D1_COLOR_F cursorCol = clickgui->getColor("primary2", "ClickGUI");

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
        MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        //ColorPickers[index].isActive = true;
        FlarialGUI::ColorPickerWindow(index, moduleName, settingName);
        activeColorPickerWindows++;
    }

}