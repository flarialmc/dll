
#include "FlarialWidgets.h"

    // TO DO: VERSION CARD
static bool CursorInRect(float rectX, float rectY, float width, float height) {
    if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height) {
        return true;
    }
    return false;
}



 bool FlarialGUI::Button(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width, const float height) {
    
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

    if(CursorInRect(x, y, width, height) && MC::mouseaction == MouseAction::Left && !MC::held)
    {
        MC::mouseaction = MouseAction::None;
        return true;
    } else
    {
        MC::mouseaction = MouseAction::None;
        return false;
    }
 }

  bool FlarialGUI::RoundedButton(const float x, const float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width, const float height,float radiusX, float radiusY) {

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

    if(CursorInRect(x, y, width, height) && MC::mouseaction == MouseAction::Left && !MC::held)
    {
        MC::mouseaction = MouseAction::None;
        return true;
    } else
    {
        MC::mouseaction = MouseAction::None;
        return false;
    }
  }

 void FlarialGUI::VersionCard(const float x, const float y, const float width, const float height)
{    
    
    RoundedRect(x, y + 10, D2D1::ColorF(47.0f/255.0f, 32.0f/255.0f, 34.0f/255.0f));
    RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f));
    
 }

void FlarialGUI::RoundedRect(const float x, const float y, const D2D_COLOR_F color, const float width, const float height,float radiusX, float radiusY) {

    ID2D1SolidColorBrush* brush;
        RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    RenderUtils::D2DC->FillRoundedRectangle(roundedRect, brush);
    brush->Release();
}

 void FlarialGUI::RoundedRectOnlyTopCorner(const float x, const float y, D2D_COLOR_F color, const float width, const float height)
 {
    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
    
    ID2D1Factory* factory;
    RenderUtils::D2DC->GetFactory(&factory);

    ID2D1PathGeometry* geometry = nullptr;
    factory->CreatePathGeometry(&geometry);

    ID2D1GeometrySink* sink = nullptr;
    geometry->Open(&sink);

    float radiusX = 10;
    float radiusY = 10;

    D2D1_POINT_2F startPoint = D2D1::Point2F(rect.left + radiusX, rect.top);
    D2D1_POINT_2F topRightArcEndPoint = D2D1::Point2F(rect.right - radiusX, rect.top);
    D2D1_POINT_2F topLeftArcEndPoint = D2D1::Point2F(rect.left, rect.top + radiusY);
    D2D1_POINT_2F endPoint = D2D1::Point2F(rect.right, rect.bottom);

    sink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

    // Top line (first segment)
    sink->AddLine(topRightArcEndPoint);

    // Top right arc
    D2D1_ARC_SEGMENT arc2;
    arc2.point = D2D1::Point2F(rect.right, rect.top + radiusY);
    arc2.size = D2D1::SizeF(radiusX, radiusY);
    arc2.rotationAngle = 90.0f;
    arc2.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc2.arcSize = D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc2);

    // Top line (second segment)
    sink->AddLine(endPoint);

    // Right line
    sink->AddLine(D2D1::Point2F(rect.right, rect.bottom));

    // Bottom line
    sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));

    // Left line
    sink->AddLine(D2D1::Point2F(rect.left, rect.top + radiusY));

    // Bottom left arc
    D2D1_ARC_SEGMENT arc3;
    arc3.point = startPoint;
    arc3.size = D2D1::SizeF(radiusX, radiusY);
    arc3.rotationAngle = 90.0f;
    arc3.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc3.arcSize = D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc3);

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();


    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
    
    RenderUtils::D2DC->FillGeometry(geometry, brush);
 }


