#include "Button.hpp"
#include "../../../Engine.hpp"

bool FlarialGUI::Button(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text,
                        const float width, const float height) {
    if (isInScrollView) y += scrollpos;
    if (shouldAdditionalY) y += additionalY[additionalIndex];

    const bool isAdditionalY = shouldAdditionalY;

    D2D1_COLOR_F buttonColor = CursorInRect(x, y, width, height) ? D2D1::ColorF(color.r - darkenAmounts[x + y],
                                                                                color.g - darkenAmounts[x + y],
                                                                                color.b - darkenAmounts[x + y], color.a)
                                                                 : color;

    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);


    D2D::context->FillRectangle(rect, FlarialGUI::getBrush(buttonColor));

    FlarialGUI::FlarialTextWithFont(x, y, text, width, height, DWRITE_TEXT_ALIGNMENT_CENTER, 14.0f,
                                    DWRITE_FONT_WEIGHT_NORMAL, textColor);

    if (CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        return true;
    } else {
        MC::mouseButton = MouseButton::None;
        return false;
    }
}