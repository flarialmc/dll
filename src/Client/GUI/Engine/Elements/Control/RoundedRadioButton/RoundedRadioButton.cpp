#include "../../../Engine.hpp"
#include "../../../animations/fadeinout.hpp"

bool FlarialGUI::RoundedRadioButton(int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor,
                                    const wchar_t *text, const float width, const float height, float radiusX,
                                    float radiusY, const std::string &radioNum, const std::string &currentNum) {
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (FlarialGUI::DropDownMenus[i].isActive && i <= additionalIndex) {
                y += additionalY[i];
            }
        }
    }
    if (isInScrollView) y += scrollpos;

    const bool isAdditionalY = shouldAdditionalY;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    D2D1_COLOR_F buttonColor;

    if (radioNum != currentNum) {
        FadeEffect::ApplyFadeInEffect(0.03f * FlarialGUI::frameFactor, 1, opacityAmounts[index]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[index]);
    } else {
        FadeEffect::ApplyFadeOutEffect(0.03f * FlarialGUI::frameFactor, opacityAmounts[index]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[index]);
    }

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, FlarialGUI::getBrush(buttonColor).get());

    x += Constraints::SpacingConstraint(0.077, width);
    //D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);
    // TODO: check if this correct
    FlarialGUI::FlarialTextWithFont(x, y, text, width, height, DWRITE_TEXT_ALIGNMENT_CENTER, width * 0.84f,
                                    DWRITE_FONT_WEIGHT_REGULAR, textColor);

    if (isAdditionalY) SetIsInAdditionalYMode();

    if (CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        return true;
    }

    return false;
}