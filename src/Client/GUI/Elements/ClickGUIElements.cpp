/*#include "ClickGUIElements.hpp"


#include "../Engine/Constraints.hpp"
#include "../Engine/Engine.hpp"

void ClickGUIElements::ModCard(float x, float y, Module* mod, const std::string iconpath, const float width, const float height)
{

    Vec2<float> vec = FlarialGUI::CalculateMovedXY(x, y, 0);
    x = vec.x;
    y = vec.y;

    // Bottom rounded rect
    float BottomRoundedWidth = Constraints::RelativeConstraint(0.234f, "width");
    float BottomRoundedHeight = Constraints::RelativeConstraint(0.165f, "height");
    FlarialGUI::RoundedRect(x, y, D2D1::ColorF(47.0f/255.0f, 32.0f/255.0f, 34.0f/255.0f), BottomRoundedWidth, BottomRoundedHeight);
    

    // Top rounded rect

    float TopRoundedHeight = Constraints::RelativeConstraint(0.105f, "height");
    FlarialGUI::RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), BottomRoundedWidth, TopRoundedHeight);
    // Settings Rect



    // Mod icon


    // enabled / disabled button

    std::string text = "Enabled";
    text = mod->enabled ? "Enabled" : "Disabled";
    D2D1_COLOR_F color = D2D1::ColorF(26.0f / 255.0f, 193.0f / 255.0f, 63.0f / 255.0f);
    color = mod->enabled ? D2D1::ColorF(26.0f / 255.0f, 193.0f / 255.0f, 63.0f / 255.0f) : D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f);

    

}

*/