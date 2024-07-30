#include "../../../../Engine.hpp"
#include "../../../../../../Client.hpp"

void FlarialGUI::ShadowRect(Vec2<float> pos, Vec2<float> size, D2D_COLOR_F color, float rounding, int shadowSize)
{
    if (isInScrollView) pos.y += scrollpos;

    color.a *= .5f;

    for (int i = 0; i < shadowSize; i++)
    {
        float progress = (float)i / shadowSize;
        float alphaFactor = (1.0f - progress) * (1.0f - progress); //took help from gpt for alpha factor

        ImColor fadedShadowColor = ImColor(color.r, color.g, color.b, color.a * alphaFactor);

        Vec2 offset = Vec2(progress * shadowSize, progress * shadowSize);

        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(pos.x - (offset.x), pos.y - (offset.y)), ImVec2(pos.x + size.x + (offset.x * 2), pos.y + size.y + (offset.y * 2)), fadedShadowColor, rounding + progress * shadowSize, 0, 2);

        //FlarialGUI::RoundedHollowRect(pos.x - offset.x, pos.y - offset.y, 2.0f, fadedShadowColor, size.x + offset.x*2, size.y + offset.y*2, rounding + progress * shadowSize, rounding + progress * shadowSize);
        
        //RoundedRectBorder(pos - offset, size + offset + offset, fadedShadowColor, 2.0f, rounding + progress * shadowSize);
    }

    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y), D2DColorToImColor(color), rounding, 0, 2);

    //FlarialGUI::RoundedHollowRect(pos.x, pos.y, shadowSize, color, size.x, size.y, rounding, rounding);
    //RoundedRectBorder(pos, size, color, 1, rounding);
}