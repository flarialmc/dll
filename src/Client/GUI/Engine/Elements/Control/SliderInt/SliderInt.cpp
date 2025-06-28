#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

using namespace winrt::Windows::UI::Core;
int FlarialGUI::SliderInt(int index, float x, float y, int& startingPoint, const int maxValue, const int minValue, std::string moduleName, std::string settingName) {
	D2D1_COLOR_F color = ClickGUI::getColor("primary1");
	D2D1_COLOR_F disabledColor = ClickGUI::getColor("primary3");
	D2D1_COLOR_F circleColor = ClickGUI::getColor("primary2");

	color.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
	disabledColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;
	circleColor.a *= clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value;

	if (ClickGUI::settingsOpacity != 1) {
		color.a = ClickGUI::settingsOpacity;
		disabledColor.a = ClickGUI::settingsOpacity;
		circleColor.a = ClickGUI::settingsOpacity;
	}

	if (shouldAdditionalY) {
		for (int i = 0; i < highestAddIndexes + 1; i++) {
			if (i <= additionalIndex && additionalY[i] > 0.0f) {
				y += additionalY[i];
			}
		}
	}

	// Define the total slider rect width and height
	const bool isAdditionalY = shouldAdditionalY;
	const float totalWidth = Constraints::RelativeConstraint(0.15, "height", true);
	const float height = Constraints::RelativeConstraint(0.0045, "height", true);

	Vec2<float> round = Constraints::RoundingConstraint(13, 13);

	const float percWidth = Constraints::RelativeConstraint(0.056, "height", true);
	const float percHeight = Constraints::RelativeConstraint(0.029, "height", true);

	y -= percHeight / 2.0f;

	std::string text;

	if (isAdditionalY) UnSetIsInAdditionalYMode();

	if (!TextBoxes[30 + index].isActive) {
		text = FlarialGUI::cached_to_string(startingPoint);
	}

	float oriX = x;

	if (TextBoxes[30 + index].noCursorBgCol) {
		TextBoxes[30 + index].curBgCol = disabledColor;
		TextBoxes[30 + index].noCursorBgCol = false;
	}

	D2D_COLOR_F colToSet = TextBoxes[30 + index].isActive ? color : disabledColor;

	TextBoxes[30 + index].curBgCol = LerpColor(TextBoxes[30 + index].curBgCol, CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) ? D2D1::ColorF(colToSet.r * 0.8, colToSet.g * 0.8, colToSet.b * 0.8, colToSet.a) : colToSet, 0.1f * frameFactor);

	if (CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && !TextBoxes[30 + index].isHovering) {
		TextBoxes[30 + index].isHovering = true;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::IBeam);
	}
	else if (!CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && TextBoxes[30 + index].isHovering) {
		TextBoxes[30 + index].isHovering = false;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
	}

	RoundedRect(x, y, TextBoxes[30 + index].curBgCol, percWidth, percHeight, round.x, round.x);

	int limit = 3;

	startingPoint = std::clamp(startingPoint, minValue, maxValue);
	if (SliderIntRects[index].validInt == -1) SliderIntRects[index].validInt = std::clamp(startingPoint, minValue, maxValue);

	text = TextBox(30 + index, text, limit, x, y, percWidth, percHeight, 2, moduleName, settingName);
	text = String::removeNonNumeric(text, true);

	if (text.empty()) SliderIntRects[index].isValid = false;
	else {
		SliderIntRects[index].isValid = true;
		SliderIntRects[index].validInt = std::clamp(std::stoi(text), minValue, maxValue);
	}

	if (TextBoxes[30 + index].isActive) {
		if (TextBoxes[30 + index].isAt1) lerp(TextBoxes[30 + index].cursorOpac, -1.0f, 0.05f * frameFactor);
		else lerp(TextBoxes[30 + index].cursorOpac, 2.0f, 0.05f * frameFactor);
	}
	else TextBoxes[30 + index].cursorOpac = 0;

	if (TextBoxes[30 + index].cursorOpac > 1) TextBoxes[30 + index].isAt1 = true;
	if (TextBoxes[30 + index].cursorOpac < 0) TextBoxes[30 + index].isAt1 = false;

	std::string ok = FlarialTextWithFont(
		x + (TextBoxes[30 + index].isActive ? Constraints::SpacingConstraint(0.1, percWidth) : 0),
		y, to_wide(text).c_str(), percWidth, percHeight,
		TextBoxes[30 + index].isActive ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
		Constraints::FontScaler(percWidth * 14.5f), DWRITE_FONT_WEIGHT_NORMAL);

	D2D1_COLOR_F cursorCol = ClickGUI::getColor("primary2");

	cursorCol.a = TextBoxes[30 + index].cursorOpac;

	lerp(TextBoxes[30 + index].cursorX,
		x + TextSizes[ok] + Constraints::SpacingConstraint(0.11, percWidth),
		0.420f * frameFactor);

	// white cursor blinky

	if (TextBoxes[30 + index].cursorX > x)
		RoundedRect(
			TextBoxes[30 + index].cursorX,
			y + Constraints::RelativeConstraint(0.035f) / 3.0f, cursorCol,
			Constraints::RelativeConstraint(0.005f),
			percHeight - Constraints::RelativeConstraint(0.025f), 0, 0);

	x += Constraints::SpacingConstraint(1.2, percWidth);
	y += Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

	// Calculate the farLeftX and farRightX
	const float farLeftX = x + 4;
	float farRightX = x + totalWidth;

	// Calculate the position of the circle in the middle of the slider rect
	float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

	float circleX = x + totalWidth / 2.0f;

	float circleY;

	if (isInScrollView) circleY = (y + scrollpos) + height / 2.0f;
	else circleY = y + height / 2.0f;

	float rectangleLeft = farLeftX;
	float rectangleWidth = farRightX - farLeftX;

	if (activeColorPickerWindows) activeSliders = 1;

	if (SliderIntRects[index].hasBeenMoved) {

		if (!TextBoxes[30 + index].isActive)
			circleX = (SliderIntRects[index].validInt - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;
		else circleX = (SliderIntRects[index].validInt - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

	}
	else if (SliderIntRects[index].validInt != 50.0f && !SliderIntRects[index].hasBeenMoved)
		circleX = (SliderIntRects[index].validInt - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

	// Calculate the position and width of the enabled portion rect
	const float enabledWidth = circleX - farLeftX;

	round = Constraints::RoundingConstraint(4, 4);

	// Draw the disabled portion rect
	RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x, round.x);

	// Draw the enabled portion rect
	RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);

	if (SliderIntRects[index].isMovingElement || Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f))) {
		lerp(SliderIntRects[index].hoveredAnim, 1.f, 0.25f * frameFactor);
		if (!SliderIntRects[index].firstHover) {

			WinrtUtils::setCursorTypeThreaded(CoreCursorType::SizeWestEast);

			SliderIntRects[index].firstHover = true;
		}
	}
	else {
		if (SliderIntRects[index].firstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
			SliderIntRects[index].firstHover = false;
		}
		lerp(SliderIntRects[index].hoveredAnim, 0.f, 0.25f * frameFactor);
	}

	// Draw the circle in the middle
	Circle(circleX, circleY, color, Constraints::SpacingConstraint(circleRadius, 1.1f + (1.1f * 0.35f * SliderIntRects[index].hoveredAnim)));
	Circle(circleX, circleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55f + (0.55f * 0.35f * SliderIntRects[index].hoveredAnim)));

	// Calculate the percentage
	float percentage = ((circleX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;

	if (!clickgui->active || MC::mouseButton == MouseButton::Right) return SliderIntRects[index].percentageX;

	SliderIntRects[index].percentageX = percentage;

	if (isInScrollView) y += scrollpos;

	if (CursorInRect(farLeftX, y - (height * 3) / 2, totalWidth, height * 3) &&
		MC::held &&
		(SliderIntRects[index].isMovingElement || activeSliders == 0) &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		SliderIntRects[index].movedX = MC::mousePos.x;
		SliderIntRects[index].hasBeenMoved = true;
		SliderIntRects[index].isMovingElement = true;

		activeSliders = 1;

		percentage = ((SliderIntRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderIntRects[index].percentageX = percentage;
	}

	if (Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f)) && MC::held &&
		(activeSliders == 0 || SliderIntRects[index].isMovingElement) && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		if (MC::mousePos.x > farLeftX && MC::mousePos.x < farRightX) {

			SliderIntRects[index].movedX = MC::mousePos.x;
			SliderIntRects[index].hasBeenMoved = true;
			SliderIntRects[index].isMovingElement = true;

			activeSliders = 1;
		}

		if (MC::mousePos.x < farLeftX) SliderIntRects[index].movedX = farLeftX;
		else if (MC::mousePos.x > farRightX) SliderIntRects[index].movedX = farRightX;

		percentage = ((SliderIntRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderIntRects[index].percentageX = percentage;
	}
	else if (MC::held && SliderIntRects[index].isMovingElement && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		if (MC::mousePos.x > farLeftX && MC::mousePos.x < farRightX) {

			SliderIntRects[index].movedX = MC::mousePos.x;
			SliderIntRects[index].hasBeenMoved = true;
			SliderIntRects[index].isMovingElement = true;

		}

		if (MC::mousePos.x < farLeftX) SliderIntRects[index].movedX = farLeftX;
		else if (MC::mousePos.x > farRightX) SliderIntRects[index].movedX = farRightX;

		percentage = ((SliderIntRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderIntRects[index].percentageX = percentage;
	}

	if (MC::mouseButton == MouseButton::None && !MC::held || MC::mouseButton == MouseButton::Left && !MC::held) {
		SliderIntRects[index].isMovingElement = false;
		activeSliders = 0;
		percentage = SliderIntRects[index].percentageX;
	}

	if (isAdditionalY) SetIsInAdditionalYMode();

	if (SliderIntRects[index].isValid) startingPoint = static_cast<int>(percentage);

	return percentage;
}