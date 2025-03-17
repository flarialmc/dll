#include "../../../../Engine.hpp"

void FlarialGUI::RoundedHollowRect(float x, float y, float borderWidth, const D2D_COLOR_F color, const float width,
                                   const float height, float radiusX, float radiusY) {
    if (isInScrollView) y += scrollpos;

    // Offset the path outward by borderWidth / 2
    float adjust = borderWidth / 2.0f;
    float pathX = x - adjust;
    float pathY = y - adjust;
    float pathMaxX = x + width + adjust;
    float pathMaxY = y + height + adjust;

    // Adjust rounding for the border
    float rounding = radiusX + adjust;

    ImVec2 pos(pathX, pathY);
    ImVec2 max(pathMaxX, pathMaxY);

    ImGui::GetBackgroundDrawList()->AddRect(pos, max, D2DColorToImColor(color), rounding, 0, borderWidth);
}