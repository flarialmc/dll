#include "../../../../Engine.hpp"

void FlarialGUI::RoundedHollowRect(float x, float y, float borderWidth, const D2D_COLOR_F color, const float width,
                                   const float height, float radiusX, float radiusY) {
    if (isInScrollView) y += scrollpos;

    ID2D1SolidColorBrush *brush;
    brush = FlarialGUI::getBrush(color);
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    // Make the border extend from the outside only
    D2D1_RECT_F borderRect = D2D1::RectF(
            rect.left - borderWidth / 2.0f,
            rect.top - borderWidth / 2.0f,
            rect.right + borderWidth / 2.0f,
            rect.bottom + borderWidth / 2.0f
    );

    radiusX += borderWidth / 2.0f;
    radiusY += borderWidth / 2.0f;

    D2D::context->DrawRoundedRectangle(D2D1::RoundedRect(borderRect, radiusX, radiusY), brush, borderWidth);
}