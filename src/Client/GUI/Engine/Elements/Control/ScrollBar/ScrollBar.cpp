#include "../../../Engine.hpp"
#include "../../../../../Module/Modules/ClickGUI/GUIMouseListener.hpp"

void FlarialGUI::ScrollBar(float x, float y, float width, float height, float radius) {
    float whiteY;

    if (y - GUIMouseListener::accumilatedPos > y + height) {
        GUIMouseListener::accumilatedPos += scrollposmodifier;
    }

    if (y + GUIMouseListener::accumilatedPos > y) {
        GUIMouseListener::accumilatedPos = 0;
    }

    FlarialGUI::lerp(FlarialGUI::scrollpos, GUIMouseListener::accumilatedPos, 0.30f * FlarialGUI::frameFactor);

    /*
    // Draw the gray bar
    ID2D1SolidColorBrush *graybrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 0.0), &graybrush);
    D2D1_ROUNDED_RECT grayRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radius, radius);
    D2D::context->FillRoundedRectangle(&grayRect, graybrush);
    graybrush->Release();
    // Draw the white bar
    ID2D1SolidColorBrush *whitebrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 0.0), &whitebrush);
    D2D1_ROUNDED_RECT whiteRect = D2D1::RoundedRect(D2D1::RectF(x, whiteY, x + width, whiteY + (height * 69.5 / 100)), radius, radius);
    D2D::context->FillRoundedRectangle(&whiteRect, whitebrush);
    whitebrush->Release();
     */
}