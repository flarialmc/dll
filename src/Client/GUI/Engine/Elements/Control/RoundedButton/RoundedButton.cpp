#include "../../../Engine.hpp"
#include "../../../animations/fadeinout.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/WinrtUtils.hpp"

static float maxDarkenAmount = 0.1;
bool once;

bool FlarialGUI::RoundedButton(const int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor,
                               const wchar_t *text, const float width, const float height, float radiusX, float radiusY,
                               bool glow) {


    if (isInScrollView) y += scrollpos;
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (i <= additionalIndex && additionalY[i] > 0.0f) {
                y += additionalY[i];
            }
        }
    }

    bool anyHovered = false;
    for (const auto& isHovered : buttonsHovered | std::views::values) {
        if (isHovered) {
            anyHovered = true;
            break;
        }
    }

    if (clickgui->active)
    {
        if (anyHovered)
        {
            if (!once)
            {
                WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Hand);
                once = true;
            }
        }
        else
        {
            if (once)
            {
                WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
                once = false;
            }
        }
    }

    const bool isAdditionalY = shouldAdditionalY;
    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return false;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    D2D1_COLOR_F buttonColor;

    if (CursorInRect(x, y, width, height)) {
        /*f(FlarialGUI::buttons[index] == true) {
            FlarialGUI::buttons[index] = false;
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Hand);
            Logger::debug("true");
        }*/
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index],
                                   color.b - darkenAmounts[index], color.a);

        buttonsHovered[index] = true;
        FadeEffect::ApplyFadeInEffect(0.0055f * FlarialGUI::frameFactor, maxDarkenAmount, darkenAmounts[index]);
    } else {
        /*if (!FlarialGUI::buttons[index] == false) {
            FlarialGUI::buttons[index] = true;
            WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType::Arrow);
            Logger::debug("false");
        }*/
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index],
                                   color.b - darkenAmounts[index], color.a);

        buttonsHovered[index] = false;
        FadeEffect::ApplyFadeOutEffect(0.0055f * FlarialGUI::frameFactor, darkenAmounts[index]);
    }

    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), D2DColorToImColor(buttonColor), radiusX);


    /*
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, FlarialGUI::getBrush(buttonColor).get());
*/

    if (CursorInRect(x, y, width, height) && glow) {
        FadeEffect::ApplyFadeInEffect(0.09f * FlarialGUI::frameFactor, 1.0f, glowAlphas[index]);

    } else {
        FadeEffect::ApplyFadeOutEffect(0.09f * FlarialGUI::frameFactor, glowAlphas[index]);
    }
    //IDWriteTextFormat* textFormat = FlarialGUI::getTextFormat(Client::settings.getSettingByName<std::string>("fontname")->value, Constraints::FontScaler(width), DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    FlarialGUI::FlarialTextWithFont(x, isInScrollView ? y - scrollpos : y, text, width, height,
                                    DWRITE_TEXT_ALIGNMENT_CENTER, width, DWRITE_FONT_WEIGHT_REGULAR, textColor);

    if (isAdditionalY) SetIsInAdditionalYMode();

    if (CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        once = false;
        return true;
    }

    return false;
}