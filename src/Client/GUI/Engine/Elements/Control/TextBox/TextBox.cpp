#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#include <winrt/Windows.UI.Core.h>
#include <algorithm>

using namespace winrt::Windows::UI::Core;

#define clickgui ModuleManager::getModule("ClickGUI")

//TODO: this defo laggs req investigation
static std::string rReal;
static std::string ttext;

std::string FlarialGUI::TextBoxVisual(int index, std::string& text, int limit, float x, float y, const std::string& real, std::string moduleName, std::string settingName) {
	if (shouldAdditionalY) {
		for (int i = 0; i < highestAddIndexes + 1; i++) {
			if (i <= additionalIndex && additionalY[i] > 0.0f) {
				y += additionalY[i];
			}
		}
	}

	if (FlarialGUI::TextBoxes[index].noCursorBgCol) {
		FlarialGUI::TextBoxes[index].noCursorBgCol = false;
		FlarialGUI::TextBoxes[index].curBgCol = ClickGUI::getColor("primary3");
	}

	if (FlarialGUI::TextBoxes[index].isActive) {
		if (FlarialGUI::TextBoxes[index].isAt1) FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
		else FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
	}
	else FlarialGUI::TextBoxes[index].cursorOpac = 0;

	if (FlarialGUI::TextBoxes[index].cursorOpac > 1) FlarialGUI::TextBoxes[index].isAt1 = true;
	if (FlarialGUI::TextBoxes[index].cursorOpac < 0) FlarialGUI::TextBoxes[index].isAt1 = false;

	D2D1_COLOR_F col;

	Vec2<float> round = Constraints::RoundingConstraint(13, 13);

	const bool isAdditionalY = shouldAdditionalY;
	const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
	const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

	ImVec2 TextSize = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(text).c_str(),
		Constraints::SpacingConstraint(1.85, textWidth), percHeight,
		DWRITE_TEXT_ALIGNMENT_LEADING, textWidth, DWRITE_FONT_WEIGHT_NORMAL);

	y -= percHeight / 2.0f;

	if (isAdditionalY) UnSetIsInAdditionalYMode();

	text = FlarialGUI::TextBox(index, text, limit, x, y, Constraints::SpacingConstraint(1.85, textWidth), percHeight, 0, moduleName, settingName);

	D2D_COLOR_F bgCol = TextBoxes[index].isActive ? ClickGUI::getColor("primary1") : ClickGUI::getColor("primary3");

	TextBoxes[index].curBgCol = FlarialGUI::LerpColor(TextBoxes[index].curBgCol, CursorInRect(x, y + (isInScrollView ? scrollpos: 0), Constraints::SpacingConstraint(1.85, textWidth), percHeight) ? D2D1::ColorF(bgCol.r * 0.8, bgCol.g * 0.8, bgCol.b * 0.8, bgCol.a) : bgCol, 0.10f * FlarialGUI::frameFactor);

	TextBoxes[index].curBgCol.a = clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

	if (CursorInRect(x, y + (isInScrollView ? scrollpos : 0), Constraints::SpacingConstraint(1.85, textWidth), percHeight) && !TextBoxes[index].isHovering) {
		TextBoxes[index].isHovering = true;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::IBeam);
	}
	else if (!CursorInRect(x, y + (isInScrollView ? scrollpos : 0), Constraints::SpacingConstraint(1.85, textWidth), percHeight) && TextBoxes[index].isHovering) {
		TextBoxes[index].isHovering = false;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
	}

	if (ClickGUI::settingsOpacity != 1) col.a = ClickGUI::settingsOpacity;

	// rectangle bounds
	FlarialGUI::RoundedRect(x, y, TextBoxes[index].curBgCol, Constraints::SpacingConstraint(1.85, textWidth), percHeight, round.x, round.x);

	DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;

	bool exceeding = false;

	if (TextSize.x + Constraints::SpacingConstraint(0.2, textWidth) > Constraints::SpacingConstraint(1.85, textWidth))
	{
		alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
		exceeding = true;
	}

	FlarialGUI::PushImClipRect(ImVec2(x + Constraints::SpacingConstraint(0.1, textWidth), isInScrollView ? y + FlarialGUI::scrollpos : y), ImVec2(Constraints::SpacingConstraint(1.65, textWidth), percHeight));

	float textXOffset = x + Constraints::SpacingConstraint(0.1, textWidth);

	if (exceeding) {
		std::string TempText = text;
		TextCursorPosition = std::min((int)TempText.length(), TextCursorPosition);
		float CursorTextOffset = getFlarialTextSize(FlarialGUI::to_wide(TempText.erase(TempText.length() - TextCursorPosition)).c_str(),
			Constraints::SpacingConstraint(1.65, textWidth), percHeight,
			DWRITE_TEXT_ALIGNMENT_LEADING, textWidth, DWRITE_FONT_WEIGHT_NORMAL).x;

		float visibleWidth = Constraints::SpacingConstraint(1.65, textWidth);
		float cursorScreenX = CursorTextOffset - FlarialGUI::TextBoxes[index].scrollOffset;

		if (cursorScreenX > visibleWidth - Constraints::SpacingConstraint(0.05, textWidth)) {
			FlarialGUI::TextBoxes[index].scrollOffset = CursorTextOffset - visibleWidth + Constraints::SpacingConstraint(0.05, textWidth);
		}
		else if (cursorScreenX < Constraints::SpacingConstraint(0.15, textWidth)) {
			FlarialGUI::TextBoxes[index].scrollOffset = CursorTextOffset - Constraints::SpacingConstraint(0.15, textWidth);
		}

		FlarialGUI::TextBoxes[index].scrollOffset = std::max(0.0f, FlarialGUI::TextBoxes[index].scrollOffset);

		textXOffset -= FlarialGUI::TextBoxes[index].scrollOffset;
		alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	} else {
		FlarialGUI::TextBoxes[index].scrollOffset = 0.0f;
	}

	ttext = FlarialGUI::FlarialTextWithFont(textXOffset, y, FlarialGUI::to_wide(text).c_str(),
		Constraints::SpacingConstraint(1.65, textWidth), percHeight,
		alignment, textWidth, DWRITE_FONT_WEIGHT_NORMAL);

	FlarialGUI::PopImClipRect();

	std::string TempText = text;

	TextCursorPosition = std::min((int)TempText.length(), TextCursorPosition);
	float CursorTextOffset = getFlarialTextSize(FlarialGUI::to_wide(TempText.erase(TempText.length() - TextCursorPosition)).c_str(),
		Constraints::SpacingConstraint(1.65, textWidth), percHeight,
		DWRITE_TEXT_ALIGNMENT_LEADING, textWidth, DWRITE_FONT_WEIGHT_NORMAL).x;

	D2D1_COLOR_F cursorCol = ClickGUI::getColor("primary2");

	cursorCol.a = FlarialGUI::TextBoxes[index].cursorOpac;

	float actualCursorX;
	if (exceeding) {
		actualCursorX = x + Constraints::SpacingConstraint(0.1, textWidth) + CursorTextOffset - FlarialGUI::TextBoxes[index].scrollOffset;
	} else {
		actualCursorX = x + CursorTextOffset + Constraints::SpacingConstraint(0.11, textWidth);
	}

	FlarialGUI::lerp(FlarialGUI::TextBoxes[index].cursorX, actualCursorX, 0.420f * FlarialGUI::frameFactor);

	float visibleLeft = x + Constraints::SpacingConstraint(0.1, textWidth);
	float visibleRight = x + Constraints::SpacingConstraint(1.75, textWidth);

	if (FlarialGUI::TextBoxes[index].cursorX >= visibleLeft && FlarialGUI::TextBoxes[index].cursorX <= visibleRight)
	{
		FlarialGUI::RoundedRect(FlarialGUI::TextBoxes[index].cursorX,
			y + Constraints::RelativeConstraint(0.035f) / 2.0f, cursorCol,
			Constraints::RelativeConstraint(0.005f),
			percHeight - Constraints::RelativeConstraint(0.032f), 0, 0);
	}

	rReal = FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(0.1, textWidth), y,
		FlarialGUI::to_wide(real).c_str(),
		Constraints::SpacingConstraint(6.9, textWidth), percHeight,
		alignment, Constraints::SpacingConstraint(1.00, textWidth),
		DWRITE_FONT_WEIGHT_NORMAL);

	if (isAdditionalY) SetIsInAdditionalYMode();

	return "";
}

std::string FlarialGUI::TextBox(int index, std::string& text, int limit, float x, float y, float width, float height, int special, std::string moduleName, std::string settingName) {
	if (isInScrollView) y += scrollpos;
	if (!clickgui->active) return text;

	const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
	float textSize = Constraints::SpacingConstraint(1.0, textWidth);
	const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

	ImVec2 size = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(text).c_str(),
		Constraints::SpacingConstraint(1.85, textWidth), percHeight,
		DWRITE_TEXT_ALIGNMENT_LEADING, textSize, DWRITE_FONT_WEIGHT_NORMAL);

	if (CursorInRect(x, y, width, height) && FlarialGUI::TextBoxes[index].isActive && MC::mouseButton == MouseButton::Right && !MC::held) {
		bool resettableSettingsEnabled = Client::settings.getSettingByName<bool>("resettableSettings")->value;
		if (resettableSettingsEnabled && moduleName != "" && settingName != "") {
			auto mod = ModuleManager::getModule(moduleName);
			mod->settings.deleteSetting(settingName);
			mod->defaultConfig();
			if (special == 2) text = std::to_string(mod->settings.getSettingByName<float>(settingName)->value);
			else text = mod->settings.getSettingByName<std::string>(settingName)->value;
			FlarialGUI::TextBoxes[index].text = text;
		}
	}

	if (CursorInRect(x, y, width, height) && MC::mouseAction == MouseAction::Press &&
		MC::mouseButton == MouseButton::Left && !activeColorPickerWindows && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		FlarialGUI::TextBoxes[index].isActive = true;
		FlarialGUI::TextCursorPosition = 0;
		if (special == 2 && Client::settings.getSettingByName<bool>("clearTextBoxWhenClicked")->value) FlarialGUI::TextBoxes[index].text = "";

	}
	else if (!CursorInRect(x, y, width, height) && MC::mouseAction == MouseAction::Press &&
		MC::mouseButton == MouseButton::Left && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {
		FlarialGUI::TextBoxes[index].isActive = false;
		FlarialGUI::TextBoxes[index].text = text;
		FlarialGUI::TextCursorPosition = 0;
	}
	else if (!FlarialGUI::TextBoxes[index].isActive && !activeColorPickerWindows && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {
		FlarialGUI::TextBoxes[index].text = text;
	}

	if (FlarialGUI::TextBoxes[index].text.empty() && FlarialGUI::TextBoxes[index].firstTime) {
		FlarialGUI::TextBoxes[index].firstTime = false;
		FlarialGUI::TextBoxes[index].text = text;
	}

	if (special == 1) FlarialGUI::TextBoxes[index].text = FlarialGUI::TextBoxes[index].text.substr(0, 6);
	if (special == 2) FlarialGUI::TextBoxes[index].text = FlarialGUI::TextBoxes[index].text.substr(0, limit);
	if ((size.x > (width - 0.35 * textWidth)) && !FlarialGUI::TextBoxes[index].text.empty() && special == 3) FlarialGUI::TextBoxes[index].text.pop_back();

	return FlarialGUI::TextBoxes[index].text;
}