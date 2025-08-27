#include "../../../Engine.hpp"
#include "../../../animations/fadeinout.hpp"
#include <Utils/WinrtUtils.hpp>

bool once2 = false;

bool FlarialGUI::RoundedRadioButton(int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor,
                                    const wchar_t *text, const float width, const float height, float radiusX,
                                    float radiusY, const std::string &radioNum, const std::string &currentNum) {
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }
    if (isInScrollView) y += scrollpos;

    const bool isAdditionalY = shouldAdditionalY;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    D2D1_COLOR_F buttonColor;
    buttonColor = D2D1::ColorF(color.r, color.g, color.b, 255.f);

    FlarialGUI::RoundedRect(x, y, buttonColor, width, height, radiusX, radiusY);

    x += Constraints::SpacingConstraint(0.097, width);
    //D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);
    // TODO: check if this correct

    if(radioNum == currentNum) FlarialGUI::FlarialTextWithFont(x, y, text, width, height, DWRITE_TEXT_ALIGNMENT_CENTER, width * 1.05,
                                    DWRITE_FONT_WEIGHT_REGULAR, textColor);

    if (isAdditionalY) SetIsInAdditionalYMode();

    bool anyHovered = false;
    for (const auto& isHovered : radioButtonsHovered | std::views::values) {
        if (isHovered) {
            anyHovered = true;
            break;
        }
    }

    if (anyHovered) {
        if (!once2)
        {
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Hand);
            once2 = true;
        }
    }
    else {
        if (once2)
        {
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            once2 = false;
        }
    }

    if (CursorInRect(x, y, width, height)) radioButtonsHovered[index] = true;
    else radioButtonsHovered[index] = false;

    if (CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        once2 = false;
        return true;
    }

    return false;
}