#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "../../../animations/fadeinout.hpp"

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

bool FlarialGUI::Toggle(int index, float x, float y, bool isEnabled) {
    return FlarialGUI::Toggle(index, x, y, isEnabled, false);
}

bool FlarialGUI::Toggle(int index, float x, float y, bool isEnabled, bool rgb, std::string moduleName, std::string settingName) {
    D2D1_COLOR_F disabledColor = colors_primary3;
    D2D1_COLOR_F enabledColor = colors_primary1;
    D2D1_COLOR_F circleColor = colors_primary2;

    disabledColor.a = ClickGUI::settingsOpacity;
    enabledColor.a = ClickGUI::settingsOpacity;
    circleColor.a = ClickGUI::settingsOpacity;
    toggleColors[index].a = ClickGUI::settingsOpacity;

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }

    const bool isAdditionalY = shouldAdditionalY;
    float rectWidth = Constraints::RelativeConstraint(0.058, "height", true);
    float rectHeight = Constraints::RelativeConstraint(0.027, "height", true);

    y -= rectHeight / 2.0f;

    Vec2<float> round = Constraints::RoundingConstraint(30, 30);

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    if (isEnabled) toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index], enabledColor, 0.10f * FlarialGUI::frameFactor);
    else toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index], disabledColor, 0.10f * FlarialGUI::frameFactor);

    FlarialGUI::RoundedRect(x, y, rgb ? rgbColor : toggleColors[index], rectWidth, rectHeight, round.x, round.x);

    // the circle (I KNOW IM USING A RECT LOL)

    float circleWidth = Constraints::SpacingConstraint(0.7, rectHeight);
    float circleHeight = Constraints::SpacingConstraint(0.7, rectHeight);

    float ySpacing = Constraints::SpacingConstraint(0.2f, circleHeight);
    float xSpacing = Constraints::SpacingConstraint(0.134f, circleWidth);
    round = Constraints::RoundingConstraint(23, 23);

    float enabledSpacing;

    if (isEnabled) {
        FlarialGUI::lerp(FlarialGUI::toggleSpacings[index], Constraints::SpacingConstraint(1.6, circleWidth), 0.25f * FlarialGUI::frameFactor);
        //FadeEffect::ApplyFadeInEffect(2.4f * FlarialGUI::frameFactor, Constraints::SpacingConstraint(1.6, circleWidth),FlarialGUI::toggleSpacings[index]);
        enabledSpacing = FlarialGUI::toggleSpacings[index];
        if (enabledSpacing > Constraints::SpacingConstraint(1.6, circleWidth)) enabledSpacing = Constraints::SpacingConstraint(1.6, circleWidth);
    } else {
        FlarialGUI::lerp(FlarialGUI::toggleSpacings[index], Constraints::SpacingConstraint(0.1, circleWidth), 0.25f * FlarialGUI::frameFactor);
        //FadeEffect::ApplyFadeOutEffect(2.4f * FlarialGUI::frameFactor, FlarialGUI::toggleSpacings[index]);
        enabledSpacing = FlarialGUI::toggleSpacings[index];
    }

    FlarialGUI::RoundedRect(x + xSpacing + enabledSpacing, y + ySpacing, circleColor, circleWidth, circleHeight, round.x, round.x);

    if (isAdditionalY) SetIsInAdditionalYMode();

    if (isInScrollView) y += FlarialGUI::scrollpos;

    if (CursorInRect(x, y, rectWidth, rectHeight)) {
        if (MC::mouseButton == MouseButton::Left && !MC::held && (!activeColorPickerWindows || index == 123)) {
            MC::mouseButton = MouseButton::None;
            return true;
        }
        else if (MC::mouseButton == MouseButton::Right && !MC::held && (!activeColorPickerWindows || index == 123)) {
			if (moduleName != "nil" && settingName != "nil") {
                auto mod = ModuleManager::getModule(moduleName);
                mod->settings.deleteSetting(settingName);
				mod->defaultConfig();
				return mod->settings.getSettingByName<bool>(settingName)->value;
			}
        }
    }

    return false;
}