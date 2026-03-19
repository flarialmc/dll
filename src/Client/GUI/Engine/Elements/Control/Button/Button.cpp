#include "../../../Engine.hpp"
#include "../Utils/WinrtUtils.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

bool FlarialGUI::Button(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text,
                        const float width, const float height) {
    if (isInScrollView) y += scrollpos;
    if (shouldAdditionalY) y += additionalY[additionalIndex];

    // Simple hover darkening effect without using array indices
    D2D1_COLOR_F buttonColor = color;
    if (CursorInRect(x, y, width, height)) {
        const float darkenAmount = 0.1f;
        buttonColor = D2D1::ColorF(
            std::max(0.0f, color.r - darkenAmount),
            std::max(0.0f, color.g - darkenAmount),
            std::max(0.0f, color.b - darkenAmount),
            color.a
        );
    }

    FlarialGUI::RoundedRect(x, y, buttonColor, width, height);

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