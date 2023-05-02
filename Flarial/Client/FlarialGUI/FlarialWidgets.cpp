
#include "../Utils/Logger.h"
#include "../Utils/RenderUtils.h"
#include "../SDK/Minecraft.h"

class FlarialGUI
{

    // TO DO: VERSION CARD
    // TO DO: ANIMATIONS WHEN HOVERING!!! (U HAVE THE IDEA BARI <3)

public:
    static const float inline darkenAmount = 0.2f;
    static void Button(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 100.0f, const float height = 160.0f)
    {
        ID2D1SolidColorBrush* brush;
        if(CursorInRect(x, y, width, height))
            RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(color.r - darkenAmount,color.g - darkenAmount,color.b - darkenAmount,color.a), &brush); 
        else
            RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
        
        D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
        RenderUtils::D2DC->FillRectangle(rect, brush);

        
        IDWriteTextFormat* textFormat;
        IDWriteFactory* writeFactory;
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
        writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormat);
        ID2D1SolidColorBrush* textBrush;
        RenderUtils::D2DC->CreateSolidColorBrush(textColor, &textBrush);

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        
        RenderUtils::D2DC->DrawTextW(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

        
        brush->Release();
        textBrush->Release();
        writeFactory->Release();
        textFormat->Release();
    }

  static void RoundedButton(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width = 160.0f, const float height = 100.0f, const float radiusX = 10.0f, const float radiusY = 10.0f)
    {
        ID2D1SolidColorBrush* brush;
        if(CursorInRect(x, y, width, height))
            RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(color.r - darkenAmount,color.g - darkenAmount,color.b - darkenAmount,color.a), &brush); 
        else
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

    static bool CursorInRect(float rectX, float rectY, float width, float height) {
        if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height) {
            return true;
        }
        return false;
    }
};
