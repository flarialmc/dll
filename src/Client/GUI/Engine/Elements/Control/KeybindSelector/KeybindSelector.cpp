#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

std::wstring formatDuration(long ms) {
	std::wostringstream woss;
	woss << ms / 1000 << L"." << (ms % 1000) / 100;
	return woss.str();
}

void FlarialGUI::KeybindSelector(const int index, float x, float y, std::string& keybind, std::string moduleName, std::string settingName) {

	if (shouldAdditionalY) {
		for (int i = 0; i < highestAddIndexes + 1; i++) {
			if (i <= additionalIndex && additionalY[i] > 0.0f) {
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
		KeybindSelectors[index].curColor = clickgui->getColor("primary3", "ClickGUI");
		KeybindSelectors[index].curColorDone = true;
	}

	D2D1_COLOR_F col = KeybindSelectors[index].isActive ? clickgui->getColor("primary1", "ClickGUI") : clickgui->getColor("primary3", "ClickGUI");

	clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
	if (ClickGUI::settingsOpacity != 1) col.a = ClickGUI::settingsOpacity;

	float texty = y;
	if (KeybindSelectors[index].isActive) {
		std::chrono::steady_clock::time_point currentOnKeyTime = std::chrono::steady_clock::now();
		auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
			currentOnKeyTime - KeybindSelectors[index].currentOnKeyTime);

		FlarialGUI::FlarialTextWithFont(x, y - (percHeight * 0.8), formatDuration(2000.0 - timeDifference.count()).c_str(), percWidth, percHeight, DWRITE_TEXT_ALIGNMENT_CENTER, textWidth, DWRITE_FONT_WEIGHT_NORMAL);

		if (timeDifference.count() > 2000) KeybindSelectors[index].isActive = false;
	}

	KeybindSelectors[index].curColor = FlarialGUI::LerpColor(KeybindSelectors[index].curColor, col, 2.0f * FlarialGUI::frameFactor);

	KeybindSelectors[index].curColor.a = ClickGUI::settingsOpacity;

	std::string text;

	if (KeybindSelectors[index].isActive) {
		if (FlarialGUI::currentKeybind != "nothing") {

			KeybindSelectors[index].oldShi = keybind;
			keybind = FlarialGUI::currentKeybind;
			KeybindSelectors[index].newShi = keybind;

		}
		else {
			FlarialGUI::currentKeybind = "";
			keybind = "";
		}
	}

	text = keybind;

	if (isAdditionalY) UnSetIsInAdditionalYMode();
	FlarialGUI::RoundedRect(x, y, KeybindSelectors[index].curColor, percWidth, percHeight, round.x, round.x);

	if (text != "" & text != " ")
		FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), percWidth, percHeight,
			DWRITE_TEXT_ALIGNMENT_CENTER, textWidth, DWRITE_FONT_WEIGHT_NORMAL);
	else  FlarialGUI::FlarialTextWithFont(x, y, L"unset", percWidth, percHeight,
		DWRITE_TEXT_ALIGNMENT_CENTER, textWidth, DWRITE_FONT_WEIGHT_NORMAL);


	if (isAdditionalY) SetIsInAdditionalYMode();

	if (isInScrollView) y += scrollpos;

	if (!activeColorPickerWindows && CursorInRect(x, y, percWidth, percHeight)) {
		if (MC::mouseButton == MouseButton::Left && !MC::held && !KeybindSelectors[index].isActive) {
			KeybindSelectors[index].isActive = true;
			KeybindSelectors[index].currentOnKeyTime = std::chrono::steady_clock::now();
			MC::mouseButton = MouseButton::None;
		}
		else if (MC::mouseButton == MouseButton::Right && !MC::held && KeybindSelectors[index].isActive && Client::settings.getSettingByName<bool>("resettableSettings")->value && moduleName != "" && settingName != "") {
			KeybindSelectors[index].isActive = false;
			auto mod = ModuleManager::getModule(moduleName);
			mod->settings.deleteSetting(settingName);
			mod->defaultConfig();
			keybind = mod->settings.getSettingByName<std::string>(settingName)->value;
		}
	}

	if (!CursorInRect(x, y, percWidth, percHeight) && MC::mouseButton == MouseButton::Left && !MC::held) {
		KeybindSelectors[index].isActive = false;
	}
}