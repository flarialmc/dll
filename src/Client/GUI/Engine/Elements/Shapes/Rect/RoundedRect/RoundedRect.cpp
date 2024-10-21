#include "../../../../Engine.hpp"

void FlarialGUI::RoundedRect(float x, float y, const D2D_COLOR_F color, const float width, const float height, float radiusX,
                        float radiusY, ImDrawFlags flags) {

    if (isInScrollView) y += scrollpos;

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), D2DColorToImColor(color), radiusX, flags);
    /*
    ID2D1SolidColorBrush *brush;
    brush = FlarialGUI::getBrush(color).get();
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);

    D2D::context->FillRoundedRectangle(roundedRect, brush);
    */
}
/*
void FlarialGUI::RoundedRect(bool imgui, float x, float y, const ImColor color, const float width, const float height, float radiusX,
    float radiusY) {
    if (isInScrollView) y += scrollpos;

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), color, radiusX);

    //D2D::context->FillRoundedRectangle(roundedRect, brush);
}
*/