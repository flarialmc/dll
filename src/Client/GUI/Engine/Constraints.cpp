
/*#include "Constraints.hpp"

#include "Engine.hpp"
#include "../SDK/Minecraft.h"

float Constraints::PercentageConstraint(float percentage, const std::string& edge, bool ignore_stack)
{
    float screenWidth = static_cast<float>(MC::windowSize.x);
    float screenHeight = static_cast<float>(MC::windowSize.y);
    float x = 0;
    float y = 0;

    // Check if there is a dimension stack and use the top element's dimensions instead
    if (!FlarialGUI::dimension_stack.empty() && !ignore_stack)
    {
        Dimension& top = FlarialGUI::dimension_stack.top();
        screenWidth = top.width;
        screenHeight = top.height;
        x = top.x;
        y = top.y;
    }

    float position = 0.0f;

    if (edge == "top")
    {
        position = percentage * screenHeight;
        position += y;
    }
    else if (edge == "bottom")
    {
        position = screenHeight - (percentage * screenHeight);
        position -= y;
    }
    else if (edge == "left") {
        position = percentage * screenWidth;
        position += x;
    }
    else if (edge == "right")
    {
        position = screenWidth - (percentage * screenWidth);
        position -= x;
    }

    return position;
}

float Constraints::RelativeConstraint(float percent, std::string dimension, bool ignore_stack)
{
    float length = 0.0f;
    float screenWidth = 0;
    float screenHeight = 0;
    if (ignore_stack || FlarialGUI::dimension_stack.empty())
    {
        screenWidth = MC::windowSize.x;
        screenHeight = MC::windowSize.y;
    }
    else
    {
        screenWidth = FlarialGUI::dimension_stack.top().width;
        screenHeight = FlarialGUI::dimension_stack.top().height;
    }

    Logger::debug(std::to_string(static_cast<int>(screenWidth)) + " " + std::to_string(static_cast<int>(screenHeight)));
    
    if (dimension == "width")
    {
        if(static_cast<int>(screenHeight) > static_cast<int>(screenWidth) && FlarialGUI::dimension_stack.empty() || static_cast<int>(screenHeight) > static_cast<int>(screenWidth) && ignore_stack)
            length = screenWidth * (percent + .25f);
        else length = screenWidth * percent;
    }
    else if (dimension == "height")
    {
        length = screenHeight * percent;
    }

    return length;
}

Vec2<float> Constraints::CenterConstraint(float width, float height, std::string axis, float xmodifier, float ymodifier, bool ignore_stack)
{
    Vec2<float> xy;

    float screenWidth;
    float screenHeight;
    if (ignore_stack || FlarialGUI::dimension_stack.empty())
    {
        screenWidth = static_cast<float>(MC::windowSize.x);
        screenHeight = static_cast<float>(MC::windowSize.y);
    }
    else
    {
        screenWidth = FlarialGUI::dimension_stack.top().width;
        screenHeight = FlarialGUI::dimension_stack.top().height;
    }

    xy.x = (screenWidth - width) / 2.0f;
    xy.y = (screenHeight - height) / 2.0f;

    if (axis == "x")
    {
        xy.x += xmodifier * (screenWidth - width) / 2.0f;
    }
    else if (axis == "y")
    {
        xy.y += ymodifier * (screenHeight - height) / 2.0f;
    }
    else if (axis == "both")
    {
        xy.x += xmodifier * (screenWidth - width) / 2.0f;
        xy.y += ymodifier * (screenHeight - height) / 2.0f;
    }

    return xy;
}

Vec2<float> Constraints::RoundingConstraint(float radiusX, float radiusY, bool ignore_stack)
{
    // Get the minimum screen dimension

    float screenWidth;
    float screenHeight;

    screenWidth = static_cast<float>(MC::windowSize.x);
    screenHeight = static_cast<float>(MC::windowSize.y);


    float minDimension = min(screenWidth, screenHeight);
    // Calculate the scaled radii
    float scaledRadiusX = radiusX * minDimension / 1920.0f;
    float scaledRadiusY = radiusY * minDimension / 1080.0f;

    // Return the scaled radii as a Vec2
    return Vec2<float>(scaledRadiusX, scaledRadiusY);
}
*/