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

float FlarialGUI::Slider(int index, float x, float y, float startingPoint, const float maxValue, const float minValue,
                         const bool zerosafe) {
    D2D1_COLOR_F color = colors_primary1_rgb ? rgbColor : colors_primary1;
    D2D1_COLOR_F disabledColor = colors_primary3_rgb ? rgbColor : colors_primary3;
    D2D1_COLOR_F circleColor = colors_primary2_rgb ? rgbColor : colors_primary2;

    color.a = o_colors_primary1;
    disabledColor.a = o_colors_primary3;
    circleColor.a = o_colors_primary2;

    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (FlarialGUI::DropDownMenus[i].isActive && i <= additionalIndex) {
                y += additionalY[i];
            }
        }
    }

    int i = 0;
    bool ye = false;

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

    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percWidth = Constraints::RelativeConstraint(0.056, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.029, "height", true);

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

    if (!TextBoxes[30 + index].isActive)
        FlarialGUI::RoundedRect(x, y, disabledColor, percWidth, percHeight, round.x, round.x);
    else FlarialGUI::RoundedRect(x, y, color, percWidth, percHeight, round.x, round.x);


    int limit = 5;
    if (text.find('-') != std::string::npos) limit = 6;
    text = FlarialGUI::TextBox(30 + index, text, limit, x, y, percWidth, percHeight);
    text = Utils::remomveNonNumeric(text);


    if (startingPoint > maxValue) {
        startingPoint = maxValue;
        TextBoxes[30 + index].text = std::to_string(startingPoint);
    } else if (startingPoint < minValue) {
        startingPoint = zerosafe ? 0.001f : minValue;
        TextBoxes[30 + index].text = std::to_string(startingPoint);
    } else if (!text.empty()) startingPoint = std::stof(text);

    FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), percWidth, percHeight,
                                    DWRITE_TEXT_ALIGNMENT_CENTER,
                                    Constraints::FontScaler(percWidth * 14.5f), DWRITE_FONT_WEIGHT_NORMAL);

    x += Constraints::SpacingConstraint(1.2, percWidth);
    y += Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

    // Calculate the farLeftX and farRightX
    const float farLeftX = x + 4;
    float farRightX = x + totalWidth;

    // Calculate the position of the circle in the middle of the slider rect
    const float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

    float circleX = x + totalWidth / 2.0f;

    float circleY;

    if (FlarialGUI::isInScrollView)
        circleY = (y + scrollpos) + height / 2.0f;
    else circleY = y + height / 2.0f;

    float rectangleLeft = farLeftX;
    float rectangleWidth = farRightX - farLeftX;

    if (activeColorPickerWindows) activeSliders = 1;

    if (SliderRects[index].hasBeenMoved) {

        if (!TextBoxes[30 + index].isActive)
            circleX = (SliderRects[index].percentageX - minValue) * (rectangleWidth / (maxValue - minValue)) +
                      rectangleLeft;
        else circleX = (startingPoint - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

    } else if (startingPoint != 50.0f && !SliderRects[index].hasBeenMoved) {

        circleX = (startingPoint - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

    }

    // Calculate the position and width of the enabled portion rect
    const float enabledWidth = circleX - farLeftX;

    round = Constraints::RoundingConstraint(4, 4);

    // Draw the disabled portion rect
    RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x, round.x);

    // Draw the enabled portion rect
    RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);

    round = Constraints::RoundingConstraint(13, 13);

    // Draw the circle in the middle
    FlarialGUI::Circle(circleX, circleY, color, Constraints::SpacingConstraint(circleRadius, 1.1));
    FlarialGUI::Circle(circleX, circleY, circleColor, Constraints::SpacingConstraint(circleRadius, 0.55));

    // Calculate the percentage
    float percentage = ((circleX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;

    SliderRects[index].percentageX = percentage;

    if (isInScrollView) {
        y += scrollpos;
        circleY += scrollpos;
    }

    if (CursorInRect(farLeftX, y, totalWidth, height) && MC::held &&
        (SliderRects[index].isMovingElement || activeSliders == 0)) {

        SliderRects[index].movedX = MC::mousePos.x;
        SliderRects[index].hasBeenMoved = true;
        SliderRects[index].isMovingElement = true;

        activeSliders = 1;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;

    }

    if (Utils::CursorInEllipse(circleX, circleY, Constraints::SpacingConstraint(circleRadius, 1.5f),
                        Constraints::SpacingConstraint(circleRadius, 1.5f)) && MC::held &&
        (activeSliders == 0 || SliderRects[index].isMovingElement)) {

        if (MC::mousePos.x > farLeftX && MC::mousePos.x < farRightX) {

            SliderRects[index].movedX = MC::mousePos.x;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;

            activeSliders = 1;
        }

        if (MC::mousePos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if (MC::mousePos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage =
                ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;

    } else if (MC::held && SliderRects[index].isMovingElement) {

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

    return percentage;
}