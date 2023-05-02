
#include "../Utils/Logger.h"
#include "../Utils/RenderUtils.h"

class FlarialGUI
{
public: static void Button(const float x, const float y, const D2D_COLOR_F color, const float width = 100.0f, const float height = 160.0f)
    {
        
        D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
        ID2D1SolidColorBrush* brush;
        RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
        RenderUtils::D2DC->FillRectangle(rect, brush);
        brush->Release();
    }

  static void RoundedButton(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f)
    {

    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);

    IDWriteTextFormat* textFormat;
    IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormat);
    ID2D1SolidColorBrush* textBrush;
    RenderUtils::D2DC->CreateSolidColorBrush(textColor, &textBrush);

    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    RenderUtils::D2DC->FillRoundedRectangle(roundedRect, brush);

    RenderUtils::D2DC->DrawTextW(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);
    
    brush->Release();
    textBrush->Release();
    writeFactory->Release();
    textFormat->Release();
    
    }
};
