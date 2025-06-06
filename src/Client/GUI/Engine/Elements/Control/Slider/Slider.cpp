#include "../../../Engine.hpp"
#include "../../../../../Module/Manager.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

float FlarialGUI::Slider(int index, float x, float y, float& startingPoint, const float maxValue, const float minValue, const bool zerosafe, std::string moduleName, std::string settingName) {
	D2D1_COLOR_F color = clickgui->getColor("primary1", "ClickGUI");
	D2D1_COLOR_F disabledColor = clickgui->getColor("primary3", "ClickGUI");
	D2D1_COLOR_F circleColor = clickgui->getColor("primary2", "ClickGUI");

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

	// bool ye = false;

	/*
	for (auto& rect : SliderRects) {

		if (rect.isMovingElement && i != index) {
			ye = true;
			break;
		}

		i++;
	}*/

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

	FlarialGUI::RoundedRect(x, y, TextBoxes[30 + index].isActive ? color : disabledColor, percWidth, percHeight, round.x, round.x);

	int limit = 5;
	if (text.find('-') != std::string::npos) limit = 6;

	text = FlarialGUI::TextBox(30 + index, text, limit, x, y, percWidth, percHeight, 2, moduleName, settingName);
	text = String::removeNonNumeric(text);
	if (text == ".") {
		if (zerosafe) text = "0.01";
		else text = "0.";
	}

	if (FlarialGUI::TextBoxes[30 + index].isActive) {
		if (FlarialGUI::TextBoxes[30 + index].isAt1) FlarialGUI::lerp(FlarialGUI::TextBoxes[30 + index].cursorOpac, -1.0f, 0.05f * FlarialGUI::frameFactor);
		else FlarialGUI::lerp(FlarialGUI::TextBoxes[30 + index].cursorOpac, 2.0f, 0.05f * FlarialGUI::frameFactor);
	}
	else {
		FlarialGUI::TextBoxes[30 + index].cursorOpac = 0;
	}

	if (FlarialGUI::TextBoxes[30 + index].cursorOpac > 1) FlarialGUI::TextBoxes[30 + index].isAt1 = true;
	if (FlarialGUI::TextBoxes[30 + index].cursorOpac < 0) FlarialGUI::TextBoxes[30 + index].isAt1 = false;

	if (startingPoint > maxValue) {
		startingPoint = maxValue;
		TextBoxes[30 + index].text = FlarialGUI::cached_to_string(startingPoint);
	}
	else if (startingPoint < minValue) {
		startingPoint = zerosafe ? 0.001f : minValue;
		TextBoxes[30 + index].text = FlarialGUI::cached_to_string(startingPoint);
	}
	else if (!text.empty()) startingPoint = std::stof(text);

	std::string ok = FlarialGUI::FlarialTextWithFont(
		x + (FlarialGUI::TextBoxes[30 + index].isActive ? Constraints::SpacingConstraint(0.1, percWidth) : 0),
		y, FlarialGUI::to_wide(text).c_str(), percWidth, percHeight,
		FlarialGUI::TextBoxes[30 + index].isActive ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
		Constraints::FontScaler(percWidth * 14.5f), DWRITE_FONT_WEIGHT_NORMAL);

	D2D1_COLOR_F cursorCol = clickgui->getColor("primary2", "ClickGUI");

	cursorCol.a = FlarialGUI::TextBoxes[30 + index].cursorOpac;

	FlarialGUI::lerp(FlarialGUI::TextBoxes[30 + index].cursorX,
		x + FlarialGUI::TextSizes[ok] + Constraints::SpacingConstraint(0.11, percWidth),
		0.420f * FlarialGUI::frameFactor);

	// white cursor blinky

	if (FlarialGUI::TextBoxes[30 + index].cursorX > x)
		FlarialGUI::RoundedRect(
			FlarialGUI::TextBoxes[30 + index].cursorX,
			y + Constraints::RelativeConstraint(0.035f) / 3.0f, cursorCol,
			Constraints::RelativeConstraint(0.005f),
			percHeight - Constraints::RelativeConstraint(0.025f), 0, 0);

	x += Constraints::SpacingConstraint(1.2, percWidth);
	y += Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

	// Calculate the farLeftX and farRightX
	const float farLeftX = x + 4;
	float farRightX = x + totalWidth;

	// Calculate the position of the circle in the middle of the slider rect
	const float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

	float circleX = x + totalWidth / 2.0f;

	float circleY;

	if (FlarialGUI::isInScrollView) circleY = (y + scrollpos) + height / 2.0f;
	else circleY = y + height / 2.0f;

	float rectangleLeft = farLeftX;
	float rectangleWidth = farRightX - farLeftX;

	if (activeColorPickerWindows) activeSliders = 1;

	if (SliderRects[index].hasBeenMoved) {

		if (!TextBoxes[30 + index].isActive)
			circleX = (SliderRects[index].percentageX - minValue) * (rectangleWidth / (maxValue - minValue)) +
			rectangleLeft;
		else circleX = (startingPoint - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

	}
	else if (startingPoint != 50.0f && !SliderRects[index].hasBeenMoved) {

		circleX = (startingPoint - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

	}

	// Calculate the position and width of the enabled portion rect
	const float enabledWidth = circleX - farLeftX;

	round = Constraints::RoundingConstraint(4, 4);

	// Draw the disabled portion rect
	RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x, round.x);

	// Draw the enabled portion rect
	RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);

	// Draw the circle in the middle
	FlarialGUI::Circle(circleX, circleY, color, Constraints::SpacingConstraint(circleRadius, 1.1));
	FlarialGUI::Circle(circleX, circleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55));

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

	/* if (CursorInRect(oriX, y, percWidth, percHeight) && TextBoxes[30 + index].isActive && MC::mouseButton == MouseButton::Right && !MC::held && clickgui->settings.getSettingByName<float>("_overrideAlphaValues_")->value > 0.95f) {
		 if (Client::settings.getSettingByName<bool>("resettableSettings")->value && moduleName != "" && settingName != "") {
			 auto mod = ModuleManager::getModule(moduleName);
			 mod->settings.deleteSetting(settingName);
			 mod->defaultConfig();
			 text = std::to_string(mod->settings.getSettingByName<float>(settingName)->value);
		 }
	 }*/

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

	std::stringstream adjustedFloat;

	adjustedFloat << std::fixed << std::setprecision(3) << percentage;

	startingPoint = std::stof(adjustedFloat.str());

	return std::stof(adjustedFloat.str());
}
