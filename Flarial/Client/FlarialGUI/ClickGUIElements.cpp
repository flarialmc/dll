#include "ClickGUIElements.h"
void ClickGUIElements::ModCard(float x, float y, Module* mod, const int currentNum, const float width, const float height)
{

    Vec2<float> vec = FlarialGUI::CalculateMovedXY(x, y, 0);
    x = vec.x;
    y = vec.y;
    FlarialGUI::RoundedRect(x, y + 35, D2D1::ColorF(47.0f/255.0f, 32.0f/255.0f, 34.0f/255.0f), 150.6f);
    FlarialGUI::RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), 150, 75);
    FlarialGUI::RoundedRectWithImageAndText(x + 10, y + 80, width, height, D2D1::ColorF(112.0f / 255.0f, 93.0f / 255.0f, 96.0f / 255.0f), L"gear.png", width, height, L"");
    std::string text = "Enabled";
    text = mod->enabled ? "Enabled" : "Disabled";
    D2D1_COLOR_F color = D2D1::ColorF(26.0f / 255.0f, 193.0f / 255.0f, 63.0f / 255.0f);
    color = mod->enabled ? D2D1::ColorF(26.0f / 255.0f, 193.0f / 255.0f, 63.0f / 255.0f) : D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f);
    if(FlarialGUI::RoundedButton(x + 42, y + 80, color, D2D1::ColorF(D2D1::ColorF::White), FlarialGUI::to_wide(text).c_str(), 102.2f, 26.5f, 6, 6))
    {
        mod->enabled = !mod->enabled;
        mod->enabled ? D2D1::ColorF(26.0f / 255.0f, 193.0f / 255.0f, 63.0f / 255.0f) : D2D1::ColorF(139.0f / 255.0f, 27.0f / 255.0f, 37.0f / 255.0f);
        Logger::debug(std::to_string(mod->enabled));
    }

    FlarialGUI::FlarialText(x, y, FlarialGUI::to_wide(mod->name).c_str(), D2D1::ColorF(D2D1::ColorF::White), 100, height);
}