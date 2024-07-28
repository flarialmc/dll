#include "../../../Engine.hpp"
#include "../../../animations/fadeinout.hpp"

static float maxDarkenAmount = 0.1;

bool FlarialGUI::RoundedButton(const int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor,
                               const wchar_t *text, const float width, const float height, float radiusX, float radiusY,
                               bool glow) {
    if (isInScrollView) y += scrollpos;
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (FlarialGUI::DropDownMenus[i].isActive && i <= additionalIndex) {
                y += additionalY[i];
            }
        }
    }

    const bool isAdditionalY = shouldAdditionalY;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return false;

    D2D1_COLOR_F buttonColor;

    if (CursorInRect(x, y, width, height)) {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index],
                                   color.b - darkenAmounts[index], color.a);
        FadeEffect::ApplyFadeInEffect(0.0055f * FlarialGUI::frameFactor, maxDarkenAmount, darkenAmounts[index]);
    } else {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index],
                                   color.b - darkenAmounts[index], color.a);
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

    D2D1_COLOR_F allahColor = FlarialGUI::buttonColors[index];
    allahColor.r += 0.02f;
    allahColor.g += 0.02f;
    allahColor.b += 0.02f;
    allahColor.a = glowAlphas[index];

    FlarialGUI::InnerShadowRect(D2D1::RoundedRect(D2D1::RectF(x, y, x + width * 1.035f, y + height), radiusX, radiusY),
                                25, allahColor);
    //IDWriteTextFormat* textFormat = FlarialGUI::getTextFormat(Client::settings.getSettingByName<std::string>("fontname")->value, Constraints::FontScaler(width), DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    FlarialGUI::FlarialTextWithFont(x, isInScrollView ? y - scrollpos : y, text, width, height,
                                    DWRITE_TEXT_ALIGNMENT_CENTER, width, DWRITE_FONT_WEIGHT_REGULAR, textColor);

    if (isAdditionalY) SetIsInAdditionalYMode();

    if (CursorInRect(x, y, width, height) && MC::mouseButton == MouseButton::Left && !MC::held) {
        MC::mouseButton = MouseButton::None;
        return true;

    }

    return false;
}