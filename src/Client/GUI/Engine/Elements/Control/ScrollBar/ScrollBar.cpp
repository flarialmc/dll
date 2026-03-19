#include "../../../Engine.hpp"
#include "../../../../../Module/Modules/ClickGUI/ClickGUI.hpp"

void FlarialGUI::ScrollBar(float x, float y, float width, float height, float radius) {
    float whiteY;

    // Clamp scroll to prevent scrolling above the top of content
    if (ClickGUI::accumilatedPos > 0) {
        ClickGUI::accumilatedPos = 0;
    }

    // Clamp scroll to prevent scrolling too far past the bottom of content
    // Calculate the viewport height from ScrollViewRect if available
    float viewportHeight = ScrollViewRect.bottom - ScrollViewRect.top;
    if (viewportHeight <= 0) {
        // Fallback if ScrollViewRect isn't set yet - use a reasonable default
        viewportHeight = height * 0.5f;
    }

    // Calculate minimum scroll position (how far we can scroll down)
    // If content is smaller than viewport, no scrolling is needed
    float minScroll = 0.0f;
    if (height > viewportHeight) {
        // Content is taller than viewport, allow scrolling
        // minScroll is negative because scrolling down = negative offset
        // Add extra padding (half viewport height) to allow scrolling last items fully into view
        float extraPadding = viewportHeight * 0.5f;
        minScroll = -(height - viewportHeight + extraPadding);
    }

    if (ClickGUI::accumilatedPos < minScroll) {
        ClickGUI::accumilatedPos = minScroll;
    }

    FlarialGUI::lerp(FlarialGUI::scrollpos, ClickGUI::accumilatedPos, 0.30f * FlarialGUI::frameFactor);

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