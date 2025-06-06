#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "../../../animations/fadeinout.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

void FlarialGUI::Toggle(int index, float x, float y, bool isEnabled) {
    FlarialGUI::Toggle(index, x, y, isEnabled, false);
}

void FlarialGUI::Toggle(int index, float x, float y, bool isEnabled, bool rgb, std::string moduleName, std::string settingName) {
    auto mod = ModuleManager::getModule(moduleName);

    D2D1_COLOR_F disabledColor = clickgui->getColor("primary3", "ClickGUI");
    D2D1_COLOR_F enabledColor = clickgui->getColor("primary1", "ClickGUI");
    D2D1_COLOR_F circleColor = clickgui->getColor("primary2", "ClickGUI");

    enabledColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    disabledColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    circleColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

    if (ClickGUI::settingsOpacity != 1) {
        disabledColor.a = ClickGUI::settingsOpacity;
        enabledColor.a = ClickGUI::settingsOpacity;
        circleColor.a = ClickGUI::settingsOpacity;
        toggleColors[index].a = ClickGUI::settingsOpacity;
    }

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

    if (settingName != "" ? mod->getOps<bool>(settingName) : isEnabled) toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index], enabledColor, 0.10f * FlarialGUI::frameFactor);
    else toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index], disabledColor, 0.10f * FlarialGUI::frameFactor);

    FlarialGUI::RoundedRect(x, y, rgb ? rgbColor : toggleColors[index], rectWidth, rectHeight, round.x, round.x);

    // the circle (I KNOW IM USING A RECT LOL)

    float circleWidth = Constraints::SpacingConstraint(0.7, rectHeight);
    float circleHeight = Constraints::SpacingConstraint(0.7, rectHeight);

    float ySpacing = Constraints::SpacingConstraint(0.2f, circleHeight);
    float xSpacing = Constraints::SpacingConstraint(0.134f, circleWidth);
    round = Constraints::RoundingConstraint(23, 23);

    float enabledSpacing;

    if (settingName != "" ? mod->getOps<bool>(settingName) : isEnabled) {
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

    if (!clickgui->active) return;

    if (isInScrollView) y += FlarialGUI::scrollpos;

    if (CursorInRect(x, y, rectWidth, rectHeight) && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {
        if (MC::mouseButton == MouseButton::Left && !MC::held && (!activeColorPickerWindows || index == 123)) {
            MC::mouseButton = MouseButton::None;
            if (settingName != "") mod->getOps<bool>(settingName) = !mod->getOps<bool>(settingName);
            //return true;
        }
        else if (MC::mouseButton == MouseButton::Right && !MC::held && (!activeColorPickerWindows || index == 123)) {
			if (Client::settings.getSettingByName<bool>("resettableSettings")->value && moduleName != "" && settingName != "") {
                auto mod = ModuleManager::getModule(moduleName);
                mod->settings.deleteSetting(settingName);
				mod->defaultConfig();
				mod->settings.getSettingByName<bool>(settingName)->value;
			}
        }
    }

    //if (settingName != "") mod->getOps<bool>(settingName) = false;
    //return false;
}



bool FlarialGUI::Toggle(int index, float x, float y, bool isEnabled, bool rgb) {
    D2D1_COLOR_F disabledColor = clickgui->getColor("primary3", "ClickGUI");
    D2D1_COLOR_F enabledColor = clickgui->getColor("primary1", "ClickGUI");
    D2D1_COLOR_F circleColor = clickgui->getColor("primary2", "ClickGUI");

    enabledColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    disabledColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
    circleColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

    if (ClickGUI::settingsOpacity != 1) {
        disabledColor.a = ClickGUI::settingsOpacity;
        enabledColor.a = ClickGUI::settingsOpacity;
        circleColor.a = ClickGUI::settingsOpacity;
        toggleColors[index].a = ClickGUI::settingsOpacity;
    }

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
    }
    else {
        FlarialGUI::lerp(FlarialGUI::toggleSpacings[index], Constraints::SpacingConstraint(0.1, circleWidth), 0.25f * FlarialGUI::frameFactor);
        //FadeEffect::ApplyFadeOutEffect(2.4f * FlarialGUI::frameFactor, FlarialGUI::toggleSpacings[index]);
        enabledSpacing = FlarialGUI::toggleSpacings[index];
    }

    FlarialGUI::RoundedRect(x + xSpacing + enabledSpacing, y + ySpacing, circleColor, circleWidth, circleHeight, round.x, round.x);

    if (isAdditionalY) SetIsInAdditionalYMode();

    if (isInScrollView) y += FlarialGUI::scrollpos;

    if (CursorInRect(x, y, rectWidth, rectHeight) && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {
        if (MC::mouseButton == MouseButton::Left && !MC::held && (!activeColorPickerWindows || index == 123)) {
            MC::mouseButton = MouseButton::None;
            return true;
        }
    }

    return false;
}