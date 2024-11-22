#include "../../../../Engine.hpp"

void FlarialGUI::RoundedRect(float x, float y, const D2D_COLOR_F color, const float width, const float height, float radiusX,
                        float radiusY) {
    if (isInScrollView) y += scrollpos;

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

    ID2D1SolidColorBrush *brush;
    brush = FlarialGUI::getBrush(color).get();
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);

    D2D::context->FillRoundedRectangle(roundedRect, brush);
}