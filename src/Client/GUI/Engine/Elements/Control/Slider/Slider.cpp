#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

using namespace winrt::Windows::UI::Core;
float FlarialGUI::Slider(int index, float x, float y, float& startingPoint, const float maxValue, const float minValue, const bool zerosafe, std::string moduleName, std::string settingName) {
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
		std::stringstream stream;

		if (startingPoint < 10.0f) stream << std::fixed << std::setprecision(3) << startingPoint;
		else if (startingPoint < 100.0f) stream << std::fixed << std::setprecision(2) << startingPoint;
		else if (startingPoint < 1000.0f) stream << std::fixed << std::setprecision(1) << startingPoint;
		else stream << std::fixed << std::setprecision(0) << startingPoint;

		text = stream.str();
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

	int limit = 5;

	startingPoint = std::clamp(startingPoint, zerosafe && minValue == 0.f ? 0.001f : minValue, maxValue);
	if (SliderRects[index].validFloat == -1.f) SliderRects[index].validFloat = std::clamp(startingPoint, zerosafe && minValue == 0.f ? 0.001f : minValue, maxValue);

	text = TextBox(30 + index, text, limit, x, y, percWidth, percHeight, 2, moduleName, settingName);
	text = String::removeNonNumeric(text);

	if (text.empty() || text == "." || text == "-" || text == "-.") SliderRects[index].isValid = false;
	else {
		SliderRects[index].isValid = true;
		SliderRects[index].validFloat = std::clamp(std::stof(text), zerosafe && minValue == 0.f ? 0.001f : minValue, maxValue);
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

	if (SliderRects[index].hasBeenMoved) {

		if (!TextBoxes[30 + index].isActive)
			circleX = (SliderRects[index].percentageX - minValue) * (rectangleWidth / (maxValue - minValue)) +
			rectangleLeft;
		else circleX = (SliderRects[index].validFloat - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

	}
	else if (SliderRects[index].validFloat != 50.0f && !SliderRects[index].hasBeenMoved)
		circleX = (SliderRects[index].validFloat - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;



	// Calculate the position and width of the enabled portion rect
	const float enabledWidth = circleX - farLeftX;

	round = Constraints::RoundingConstraint(4, 4);

	// Draw the disabled portion rect
	RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x, round.x);

	// Draw the enabled portion rect
	RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);

	if (SliderRects[index].isMovingElement || Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f))) {
		lerp(SliderRects[index].hoveredAnim, 1.f, 0.25f * frameFactor);
		if (!SliderRects[index].firstHover) {

			WinrtUtils::setCursorTypeThreaded(CoreCursorType::SizeWestEast);

			SliderRects[index].firstHover = true;
		}
	}
	else {
		if (SliderRects[index].firstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
			SliderRects[index].firstHover = false;
		}
		lerp(SliderRects[index].hoveredAnim, 0.f, 0.25f * frameFactor);
	}

	// Draw the circle in the middle
	Circle(circleX, circleY, color, Constraints::SpacingConstraint(circleRadius, 1.1f + (1.1f * 0.35f * SliderRects[index].hoveredAnim)));
	Circle(circleX, circleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55f + (0.55f * 0.35f * SliderRects[index].hoveredAnim)));

	// Calculate the percentage
	float percentage = ((circleX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;

	if (!clickgui->active || MC::mouseButton == MouseButton::Right) return SliderRects[index].percentageX;

	SliderRects[index].percentageX = percentage;

	if (isInScrollView) y += scrollpos;

	if (CursorInRect(farLeftX, y - (height * 3) / 2, totalWidth, height * 3) &&
		MC::held &&
		(SliderRects[index].isMovingElement || activeSliders == 0) &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		SliderRects[index].movedX = MC::mousePos.x;
		SliderRects[index].hasBeenMoved = true;
		SliderRects[index].isMovingElement = true;

		activeSliders = 1;

		percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderRects[index].percentageX = percentage;

	}

	if (Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f)) && MC::held &&
		(activeSliders == 0 || SliderRects[index].isMovingElement) && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		if (MC::mousePos.x > farLeftX && MC::mousePos.x < farRightX) {

			SliderRects[index].movedX = MC::mousePos.x;
			SliderRects[index].hasBeenMoved = true;
			SliderRects[index].isMovingElement = true;

			activeSliders = 1;
		}

		if (MC::mousePos.x < farLeftX) SliderRects[index].movedX = farLeftX;
		else if (MC::mousePos.x > farRightX) SliderRects[index].movedX = farRightX;

		percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderRects[index].percentageX = percentage;
	}
	else if (MC::held && SliderRects[index].isMovingElement && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		if (MC::mousePos.x > farLeftX && MC::mousePos.x < farRightX) {

			SliderRects[index].movedX = MC::mousePos.x;
			SliderRects[index].hasBeenMoved = true;
			SliderRects[index].isMovingElement = true;

		}

		if (MC::mousePos.x < farLeftX) SliderRects[index].movedX = farLeftX;
		else if (MC::mousePos.x > farRightX) SliderRects[index].movedX = farRightX;

		percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
		SliderRects[index].percentageX = percentage;
	}

	if (MC::mouseButton == MouseButton::None && !MC::held || MC::mouseButton == MouseButton::Left && !MC::held) {
		SliderRects[index].isMovingElement = false;
		activeSliders = 0;
		percentage = SliderRects[index].percentageX;
	}

	if (percentage < 0.002f && zerosafe) percentage = 0.001f;

	if (isAdditionalY) SetIsInAdditionalYMode();

	if (SliderRects[index].isValid) startingPoint = percentage;

	if (settingName == "uiscale") {
		auto module = ModuleManager::getModule(moduleName);
		if (module && module->prevAlignments.size() != 100)
			module->prevAlignments.resize(100, DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
	}

	return percentage;
}

void FlarialGUI::RangeSlider(int index, float x, float y, float& minValueRef, float& maxValueRef, float rangeMin, float rangeMax, std::string moduleName, std::string minSettingName, std::string maxSettingName) {
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

	const bool isAdditionalY = shouldAdditionalY;
	const float totalWidth = Constraints::RelativeConstraint(0.15, "height", true);
	const float height = Constraints::RelativeConstraint(0.0045, "height", true);

	Vec2<float> round = Constraints::RoundingConstraint(13, 13);

	const float percWidth = Constraints::RelativeConstraint(0.056, "height", true);
	const float percHeight = Constraints::RelativeConstraint(0.029, "height", true);

	y -= percHeight / 2.0f;

	// Left text box (min value)
	std::string leftText;
	int leftTextBoxIndex = 60 + index * 2;
	int rightTextBoxIndex = 60 + index * 2 + 1;

	if (isAdditionalY) UnSetIsInAdditionalYMode();

	// Initialize valid floats if needed
	if (RangeSliderRects[index].leftValidFloat == -1.f)
		RangeSliderRects[index].leftValidFloat = std::clamp(minValueRef, rangeMin, rangeMax);
	if (RangeSliderRects[index].rightValidFloat == -1.f)
		RangeSliderRects[index].rightValidFloat = std::clamp(maxValueRef, rangeMin, rangeMax);

	// Left text box
	if (!TextBoxes[leftTextBoxIndex].isActive) {
		std::stringstream stream;
		if (minValueRef < 10.0f) stream << std::fixed << std::setprecision(3) << minValueRef;
		else if (minValueRef < 100.0f) stream << std::fixed << std::setprecision(2) << minValueRef;
		else if (minValueRef < 1000.0f) stream << std::fixed << std::setprecision(1) << minValueRef;
		else stream << std::fixed << std::setprecision(0) << minValueRef;
		leftText = stream.str();
	}

	if (TextBoxes[leftTextBoxIndex].noCursorBgCol) {
		TextBoxes[leftTextBoxIndex].curBgCol = disabledColor;
		TextBoxes[leftTextBoxIndex].noCursorBgCol = false;
	}

	D2D_COLOR_F leftColToSet = TextBoxes[leftTextBoxIndex].isActive ? color : disabledColor;
	TextBoxes[leftTextBoxIndex].curBgCol = LerpColor(TextBoxes[leftTextBoxIndex].curBgCol,
		CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) ?
		D2D1::ColorF(leftColToSet.r * 0.8, leftColToSet.g * 0.8, leftColToSet.b * 0.8, leftColToSet.a) : leftColToSet,
		0.1f * frameFactor);

	if (CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && !TextBoxes[leftTextBoxIndex].isHovering) {
		TextBoxes[leftTextBoxIndex].isHovering = true;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::IBeam);
	}
	else if (!CursorInRect(x, y + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && TextBoxes[leftTextBoxIndex].isHovering) {
		TextBoxes[leftTextBoxIndex].isHovering = false;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
	}

	RoundedRect(x, y, TextBoxes[leftTextBoxIndex].curBgCol, percWidth, percHeight, round.x, round.x);

	minValueRef = std::clamp(minValueRef, rangeMin, maxValueRef);
	leftText = TextBox(leftTextBoxIndex, leftText, 5, x, y, percWidth, percHeight, 2, moduleName, minSettingName);
	leftText = String::removeNonNumeric(leftText);

	if (leftText.empty() || leftText == ".") RangeSliderRects[index].leftIsValid = false;
	else {
		RangeSliderRects[index].leftIsValid = true;
		RangeSliderRects[index].leftValidFloat = std::clamp(std::stof(leftText), rangeMin, maxValueRef);
	}

	if (TextBoxes[leftTextBoxIndex].isActive) {
		if (TextBoxes[leftTextBoxIndex].isAt1) lerp(TextBoxes[leftTextBoxIndex].cursorOpac, -1.0f, 0.05f * frameFactor);
		else lerp(TextBoxes[leftTextBoxIndex].cursorOpac, 2.0f, 0.05f * frameFactor);
	}
	else TextBoxes[leftTextBoxIndex].cursorOpac = 0;

	if (TextBoxes[leftTextBoxIndex].cursorOpac > 1) TextBoxes[leftTextBoxIndex].isAt1 = true;
	if (TextBoxes[leftTextBoxIndex].cursorOpac < 0) TextBoxes[leftTextBoxIndex].isAt1 = false;

	std::string leftOk = FlarialTextWithFont(
		x + (TextBoxes[leftTextBoxIndex].isActive ? Constraints::SpacingConstraint(0.1, percWidth) : 0),
		y, to_wide(leftText).c_str(), percWidth, percHeight,
		TextBoxes[leftTextBoxIndex].isActive ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
		Constraints::FontScaler(percWidth * 14.5f), DWRITE_FONT_WEIGHT_NORMAL);

	D2D1_COLOR_F cursorCol = ClickGUI::getColor("primary2");
	cursorCol.a = TextBoxes[leftTextBoxIndex].cursorOpac;

	lerp(TextBoxes[leftTextBoxIndex].cursorX, x + TextSizes[leftOk] + Constraints::SpacingConstraint(0.11, percWidth), 0.420f * frameFactor);

	if (TextBoxes[leftTextBoxIndex].cursorX > x)
		RoundedRect(TextBoxes[leftTextBoxIndex].cursorX, y + Constraints::RelativeConstraint(0.035f) / 3.0f, cursorCol,
			Constraints::RelativeConstraint(0.005f), percHeight - Constraints::RelativeConstraint(0.025f), 0, 0);

	float sliderStartX = x + Constraints::SpacingConstraint(1.2, percWidth);
	float sliderY = y + Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

	// Calculate slider bounds
	const float farLeftX = sliderStartX + 4;
	float farRightX = sliderStartX + totalWidth;
	float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

	float sliderCircleY;
	if (isInScrollView) sliderCircleY = (sliderY + scrollpos) + height / 2.0f;
	else sliderCircleY = sliderY + height / 2.0f;

	float rectangleLeft = farLeftX;
	float rectangleWidth = farRightX - farLeftX;

	// Calculate left and right circle positions based on values
	float leftCircleX, rightCircleX;

	if (RangeSliderRects[index].leftHasBeenMoved) {
		if (!TextBoxes[leftTextBoxIndex].isActive)
			leftCircleX = (RangeSliderRects[index].leftPercentageX - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
		else
			leftCircleX = (RangeSliderRects[index].leftValidFloat - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
	}
	else {
		leftCircleX = (RangeSliderRects[index].leftValidFloat - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
	}

	if (RangeSliderRects[index].rightHasBeenMoved) {
		if (!TextBoxes[rightTextBoxIndex].isActive)
			rightCircleX = (RangeSliderRects[index].rightPercentageX - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
		else
			rightCircleX = (RangeSliderRects[index].rightValidFloat - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
	}
	else {
		rightCircleX = (RangeSliderRects[index].rightValidFloat - rangeMin) * (rectangleWidth / (rangeMax - rangeMin)) + rectangleLeft;
	}

	round = Constraints::RoundingConstraint(4, 4);

	// Draw the disabled portion rect (full track)
	RoundedRect(farLeftX, sliderY, disabledColor, totalWidth, height, round.x, round.x);

	// Draw the enabled portion rect (between the two handles)
	float enabledLeft = leftCircleX;
	float enabledWidth = rightCircleX - leftCircleX;
	if (enabledWidth > 0)
		RoundedRect(enabledLeft, sliderY, color, enabledWidth, height, round.x, round.x);

	if (activeColorPickerWindows) activeSliders = 1;

	// Left handle hover/drag
	if (RangeSliderRects[index].isMovingLeftHandle || Utils::CursorInEllipse(leftCircleX, sliderCircleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f))) {
		lerp(RangeSliderRects[index].leftHoveredAnim, 1.f, 0.25f * frameFactor);
		if (!RangeSliderRects[index].leftFirstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::SizeWestEast);
			RangeSliderRects[index].leftFirstHover = true;
		}
	}
	else {
		if (RangeSliderRects[index].leftFirstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
			RangeSliderRects[index].leftFirstHover = false;
		}
		lerp(RangeSliderRects[index].leftHoveredAnim, 0.f, 0.25f * frameFactor);
	}

	// Right handle hover/drag
	if (RangeSliderRects[index].isMovingRightHandle || Utils::CursorInEllipse(rightCircleX, sliderCircleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f))) {
		lerp(RangeSliderRects[index].rightHoveredAnim, 1.f, 0.25f * frameFactor);
		if (!RangeSliderRects[index].rightFirstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::SizeWestEast);
			RangeSliderRects[index].rightFirstHover = true;
		}
	}
	else {
		if (RangeSliderRects[index].rightFirstHover) {
			WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
			RangeSliderRects[index].rightFirstHover = false;
		}
		lerp(RangeSliderRects[index].rightHoveredAnim, 0.f, 0.25f * frameFactor);
	}

	// Draw left handle
	Circle(leftCircleX, sliderCircleY, color, Constraints::SpacingConstraint(circleRadius, 1.1f + (1.1f * 0.35f * RangeSliderRects[index].leftHoveredAnim)));
	Circle(leftCircleX, sliderCircleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55f + (0.55f * 0.35f * RangeSliderRects[index].leftHoveredAnim)));

	// Draw right handle
	Circle(rightCircleX, sliderCircleY, color, Constraints::SpacingConstraint(circleRadius, 1.1f + (1.1f * 0.35f * RangeSliderRects[index].rightHoveredAnim)));
	Circle(rightCircleX, sliderCircleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55f + (0.55f * 0.35f * RangeSliderRects[index].rightHoveredAnim)));

	// Calculate percentages
	float leftPercentage = ((leftCircleX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;
	float rightPercentage = ((rightCircleX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;

	if (!clickgui->active || MC::mouseButton == MouseButton::Right) {
		RangeSliderRects[index].leftPercentageX = leftPercentage;
		RangeSliderRects[index].rightPercentageX = rightPercentage;
		if (RangeSliderRects[index].leftIsValid) minValueRef = leftPercentage;
		if (RangeSliderRects[index].rightIsValid) maxValueRef = rightPercentage;
		return;
	}

	RangeSliderRects[index].leftPercentageX = leftPercentage;
	RangeSliderRects[index].rightPercentageX = rightPercentage;

	float handleY = isInScrollView ? sliderY + scrollpos : sliderY;

	// Handle left handle dragging
	if (Utils::CursorInEllipse(leftCircleX, sliderCircleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f)) && MC::held &&
		(activeSliders == 0 || RangeSliderRects[index].isMovingLeftHandle) && !RangeSliderRects[index].isMovingRightHandle &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		float clampedX = std::clamp(MC::mousePos.x, farLeftX, rightCircleX - circleRadius * 2);
		RangeSliderRects[index].leftMovedX = clampedX;
		RangeSliderRects[index].leftHasBeenMoved = true;
		RangeSliderRects[index].isMovingLeftHandle = true;
		activeSliders = 1;

		leftPercentage = ((clampedX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;
		RangeSliderRects[index].leftPercentageX = leftPercentage;
	}
	else if (MC::held && RangeSliderRects[index].isMovingLeftHandle && !RangeSliderRects[index].isMovingRightHandle &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		float clampedX = std::clamp(MC::mousePos.x, farLeftX, rightCircleX - circleRadius * 2);
		RangeSliderRects[index].leftMovedX = clampedX;
		RangeSliderRects[index].leftHasBeenMoved = true;

		leftPercentage = ((clampedX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;
		RangeSliderRects[index].leftPercentageX = leftPercentage;
	}

	// Handle right handle dragging
	if (Utils::CursorInEllipse(rightCircleX, sliderCircleY, Constraints::SpacingConstraint(circleRadius, 1.5f), Constraints::SpacingConstraint(circleRadius, 1.5f)) && MC::held &&
		(activeSliders == 0 || RangeSliderRects[index].isMovingRightHandle) && !RangeSliderRects[index].isMovingLeftHandle &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		float clampedX = std::clamp(MC::mousePos.x, leftCircleX + circleRadius * 2, farRightX);
		RangeSliderRects[index].rightMovedX = clampedX;
		RangeSliderRects[index].rightHasBeenMoved = true;
		RangeSliderRects[index].isMovingRightHandle = true;
		activeSliders = 1;

		rightPercentage = ((clampedX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;
		RangeSliderRects[index].rightPercentageX = rightPercentage;
	}
	else if (MC::held && RangeSliderRects[index].isMovingRightHandle && !RangeSliderRects[index].isMovingLeftHandle &&
		clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {

		float clampedX = std::clamp(MC::mousePos.x, leftCircleX + circleRadius * 2, farRightX);
		RangeSliderRects[index].rightMovedX = clampedX;
		RangeSliderRects[index].rightHasBeenMoved = true;

		rightPercentage = ((clampedX - rectangleLeft) / rectangleWidth) * (rangeMax - rangeMin) + rangeMin;
		RangeSliderRects[index].rightPercentageX = rightPercentage;
	}

	if (MC::mouseButton == MouseButton::None && !MC::held || MC::mouseButton == MouseButton::Left && !MC::held) {
		RangeSliderRects[index].isMovingLeftHandle = false;
		RangeSliderRects[index].isMovingRightHandle = false;
		activeSliders = 0;
		leftPercentage = RangeSliderRects[index].leftPercentageX;
		rightPercentage = RangeSliderRects[index].rightPercentageX;
	}

	if (isAdditionalY) SetIsInAdditionalYMode();

	if (RangeSliderRects[index].leftIsValid) minValueRef = leftPercentage;
	if (RangeSliderRects[index].rightIsValid) maxValueRef = rightPercentage;

	// Right text box (max value) - positioned after slider
	float rightTextBoxX = sliderStartX + totalWidth + Constraints::SpacingConstraint(0.2, percWidth);
	float rightTextBoxY = y;

	std::string rightText;
	if (!TextBoxes[rightTextBoxIndex].isActive) {
		std::stringstream stream;
		if (maxValueRef < 10.0f) stream << std::fixed << std::setprecision(3) << maxValueRef;
		else if (maxValueRef < 100.0f) stream << std::fixed << std::setprecision(2) << maxValueRef;
		else if (maxValueRef < 1000.0f) stream << std::fixed << std::setprecision(1) << maxValueRef;
		else stream << std::fixed << std::setprecision(0) << maxValueRef;
		rightText = stream.str();
	}

	if (TextBoxes[rightTextBoxIndex].noCursorBgCol) {
		TextBoxes[rightTextBoxIndex].curBgCol = disabledColor;
		TextBoxes[rightTextBoxIndex].noCursorBgCol = false;
	}

	D2D_COLOR_F rightColToSet = TextBoxes[rightTextBoxIndex].isActive ? color : disabledColor;
	TextBoxes[rightTextBoxIndex].curBgCol = LerpColor(TextBoxes[rightTextBoxIndex].curBgCol,
		CursorInRect(rightTextBoxX, rightTextBoxY + (isInScrollView ? scrollpos : 0), percWidth, percHeight) ?
		D2D1::ColorF(rightColToSet.r * 0.8, rightColToSet.g * 0.8, rightColToSet.b * 0.8, rightColToSet.a) : rightColToSet,
		0.1f * frameFactor);

	if (CursorInRect(rightTextBoxX, rightTextBoxY + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && !TextBoxes[rightTextBoxIndex].isHovering) {
		TextBoxes[rightTextBoxIndex].isHovering = true;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::IBeam);
	}
	else if (!CursorInRect(rightTextBoxX, rightTextBoxY + (isInScrollView ? scrollpos : 0), percWidth, percHeight) && TextBoxes[rightTextBoxIndex].isHovering) {
		TextBoxes[rightTextBoxIndex].isHovering = false;
		WinrtUtils::setCursorTypeThreaded(CoreCursorType::Arrow);
	}

	RoundedRect(rightTextBoxX, rightTextBoxY, TextBoxes[rightTextBoxIndex].curBgCol, percWidth, percHeight, round.x, round.x);

	maxValueRef = std::clamp(maxValueRef, minValueRef, rangeMax);
	rightText = TextBox(rightTextBoxIndex, rightText, 5, rightTextBoxX, rightTextBoxY, percWidth, percHeight, 2, moduleName, maxSettingName);
	rightText = String::removeNonNumeric(rightText);

	if (rightText.empty() || rightText == ".") RangeSliderRects[index].rightIsValid = false;
	else {
		RangeSliderRects[index].rightIsValid = true;
		RangeSliderRects[index].rightValidFloat = std::clamp(std::stof(rightText), minValueRef, rangeMax);
	}

	if (TextBoxes[rightTextBoxIndex].isActive) {
		if (TextBoxes[rightTextBoxIndex].isAt1) lerp(TextBoxes[rightTextBoxIndex].cursorOpac, -1.0f, 0.05f * frameFactor);
		else lerp(TextBoxes[rightTextBoxIndex].cursorOpac, 2.0f, 0.05f * frameFactor);
	}
	else TextBoxes[rightTextBoxIndex].cursorOpac = 0;

	if (TextBoxes[rightTextBoxIndex].cursorOpac > 1) TextBoxes[rightTextBoxIndex].isAt1 = true;
	if (TextBoxes[rightTextBoxIndex].cursorOpac < 0) TextBoxes[rightTextBoxIndex].isAt1 = false;

	std::string rightOk = FlarialTextWithFont(
		rightTextBoxX + (TextBoxes[rightTextBoxIndex].isActive ? Constraints::SpacingConstraint(0.1, percWidth) : 0),
		rightTextBoxY, to_wide(rightText).c_str(), percWidth, percHeight,
		TextBoxes[rightTextBoxIndex].isActive ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
		Constraints::FontScaler(percWidth * 14.5f), DWRITE_FONT_WEIGHT_NORMAL);

	D2D1_COLOR_F rightCursorCol = ClickGUI::getColor("primary2");
	rightCursorCol.a = TextBoxes[rightTextBoxIndex].cursorOpac;

	lerp(TextBoxes[rightTextBoxIndex].cursorX, rightTextBoxX + TextSizes[rightOk] + Constraints::SpacingConstraint(0.11, percWidth), 0.420f * frameFactor);

	if (TextBoxes[rightTextBoxIndex].cursorX > rightTextBoxX)
		RoundedRect(TextBoxes[rightTextBoxIndex].cursorX, rightTextBoxY + Constraints::RelativeConstraint(0.035f) / 3.0f, rightCursorCol,
			Constraints::RelativeConstraint(0.005f), percHeight - Constraints::RelativeConstraint(0.025f), 0, 0);
}