
#include "Constraints.hpp"
#include "Engine.hpp"

float Constraints::PercentageConstraint(float percentage, const std::string& edge, bool ignore_stack)
{
    float screenWidth = MC::windowSize.x;
    float screenHeight = MC::windowSize.y;
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

    float position;

    if (edge == "top")
    {
        position = percentage * screenHeight;
        position += y;
    }
    else if (edge == "bottom")
    {
        position = screenHeight - (percentage * screenHeight);
        position += y;
    }
    else if (edge == "left") {
        position = percentage * screenWidth;
        position += x;
    }
    else if (edge == "right")
    {
        position = screenWidth - (percentage * screenWidth);
        position += x;
    }

    return position;
}

float Constraints::RelativeConstraint(float percent, const std::string& dimension, bool ignore_stack)
{
    float length;
    float screenWidth;
    float screenHeight;

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


    
    if (dimension == "width")
    {
        length = screenWidth * percent;
    }
    else if (dimension == "height")
    {
        length = screenHeight * percent;
    }

    return length;
}

Vec2<float> Constraints::CenterConstraint(float width, float height, const std::string& axis, float xmodifier, float ymodifier, bool ignore_stack)
{
    Vec2<float> xy;

    float screenWidth;
    float screenHeight;
    float parentX = 0;
    float parentY = 0;

    if (ignore_stack || FlarialGUI::dimension_stack.empty())
    {
        screenWidth = MC::windowSize.x;
        screenHeight = MC::windowSize.y;
    }
    else
    {
        screenWidth = FlarialGUI::dimension_stack.top().width;
        screenHeight = FlarialGUI::dimension_stack.top().height;
        parentX = FlarialGUI::dimension_stack.top().x;
        parentY = FlarialGUI::dimension_stack.top().y;
    }

    xy.x = (screenWidth - width) / 2.0f + parentX;
    xy.y = (screenHeight - height) / 2.0f + parentY;

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

    screenWidth = MC::windowSize.x;
    screenHeight = MC::windowSize.y;


    float minDimension = min(screenWidth, screenHeight);
    // Calculate the scaled radii
    float scaledRadiusX = radiusX * minDimension / 1920.0f;
    float scaledRadiusY = radiusY * minDimension / 1080.0f;

    // Return the scaled radii as a Vec2
    return {scaledRadiusX, scaledRadiusY};
}

float Constraints::FontScaler(float dimension) {

    // Define the initial font size
    float initialFontSize = 14.0f;

// Calculate the scaling factor based on the new window dimensions
    float scalingFactor = dimension / 95.0f;

// Calculate the new font size
    return initialFontSize * scalingFactor;

}

Vec2<float> Constraints::CalculatePercentage(float x, float y) {

    float containerWidth = MC::windowSize.x;
    float containerHeight = MC::windowSize.y;

    float percentageX = x / containerWidth;
    float percentageY = y / containerHeight;

    return { percentageX, percentageY };
}

float Constraints::SpacingConstraint(float percentage, float dimension) {

    return dimension * percentage;

}