
#include "FlarialWidgets.h"

#include "../Manager/Module/Module.h"

// TO DO: VERSION CARD
static bool CursorInRect(float rectX, float rectY, float width, float height) {
    if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height) {
        return true;
    }
    return false;
}



 bool FlarialGUI::Button(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width, const float height) {

        if(isInScrollView && y + scrollpos < y) y += scrollpos;
    
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

  bool FlarialGUI::RoundedButton(const float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t* text, const float width, const float height,float radiusX, float radiusY) {

    if(isInScrollView && y + scrollpos > y) y += scrollpos;
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

void FlarialGUI::RoundedRect(float x, float y, const D2D_COLOR_F color, const float width, const float height,float radiusX, float radiusY) {

    
    if(isInScrollView && y + scrollpos < y) y += scrollpos;
    
    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    RenderUtils::D2DC->FillRoundedRectangle(roundedRect, brush);
    brush->Release();
}

 void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, const float width, const float height)
 {
    
    if(isInScrollView && y + scrollpos > y) y += scrollpos;
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

void FlarialGUI::RoundedRectWithImageAndText(const float x, float y, const float width, const float height, const D2D1_COLOR_F color, const wchar_t* imagePath, const int imageWidth, const int imageHeight, const wchar_t* text) {

    if(isInScrollView && y + scrollpos > y) y += scrollpos;
    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    RenderUtils::D2DC->FillRoundedRectangle(roundedRect, brush);

    Image("gear.png", roundedRect.rect, imageWidth, imageHeight);

    // Draw text
    IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    RenderUtils::D2DC->DrawTextW(text, (UINT32)wcslen(text), textFormat, textRect, brush);
}

void FlarialGUI::Image(const std::string imageName, const D2D1_RECT_F rect, const int imageWidth, const int imageHeight)
{
    ID2D1Bitmap* bitmap;
    std::string among = Utils::getRoamPath() + "\\" + imageName;
    LoadImageFromFile(to_wide(among).c_str(), &bitmap);
    
    // Draw image
    D2D1_SIZE_F imageSize = bitmap->GetSize();
    imageSize.width = imageWidth;
    imageSize.height = imageHeight;
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.left + imageSize.width, rect.top + imageSize.height);
    RenderUtils::D2DC->DrawBitmap(bitmap, imageRect);
}

void FlarialGUI::FlarialText(const float x, float y, const wchar_t* text, D2D1_COLOR_F color, const float width, const float height)
{
    if(isInScrollView && y + scrollpos > y) y += scrollpos;
    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);

    IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));
    IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    RenderUtils::D2DC->DrawTextW(text, (UINT32)wcslen(text), textFormat, textRect, brush);
    
}

void FlarialGUI::LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap)
{
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
    IWICBitmapDecoder* bitmapDecoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* formatConverter = nullptr;

    IWICImagingFactory* imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

    // Create decoder
    imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);

    // Get first frame
    bitmapDecoder->GetFrame(0, &frame);

    // Convert format to 32bppPBGRA
    imagingFactory->CreateFormatConverter(&formatConverter);
    formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);

    // Create bitmap
    RenderUtils::D2DC->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);

    
}

void FlarialGUI::SetScrollView(float x, float y, float width, float height)
{
    isInScrollView = true;
    D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
    RenderUtils::D2DC->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void FlarialGUI::UnsetScrollView()
{
    isInScrollView = false;
    RenderUtils::D2DC->PopAxisAlignedClip();
}

Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight)
{
    Vec2<float> xy;
    xy.x = (RenderUtils::D2DC->GetSize().width - rectWidth)/ 2.0f;
    xy.y = (RenderUtils::D2DC->GetSize().height - rectHeight) / 2.0f;
    return xy;
}

void FlarialGUI::ScrollBar(float x, float y, float width, float height, float radius)
{
    float whiteY;
    
    if(y - barscrollpos < y + (height * 30.5/100)) whiteY = y - (barscrollpos);
    else
    {
        whiteY = y + (height * 30.5/100);
        barscrollpos += barscrollposmodifier;
        scrollpos += scrollposmodifier;
    }

    if(y + barscrollpos > y)
    {
        whiteY = y;
        barscrollpos = 0;
        scrollpos = 0;
    }

     
    // Draw the gray bar
    ID2D1SolidColorBrush* graybrush;
    RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &graybrush);
    D2D1_ROUNDED_RECT grayRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radius, radius);
    RenderUtils::D2DC->FillRoundedRectangle(&grayRect, graybrush);
    graybrush->Release();
    // Draw the white bar
    ID2D1SolidColorBrush* whitebrush;
    RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whitebrush);
    D2D1_ROUNDED_RECT whiteRect = D2D1::RoundedRect(D2D1::RectF(x, whiteY, x + width, whiteY + (height * 69.5/100)), radius, radius);
    RenderUtils::D2DC->FillRoundedRectangle(&whiteRect, whitebrush);
    whitebrush->Release();
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum)
{
    isInWindowRect = true;
    if(!WindowRects[currentNum].madeRect)
    {
        Logger::debug(std::to_string(WindowRects[currentNum].madeRect));
        WindowRects[currentNum] = WindowRect();
        WindowRects[currentNum].madeRect = true;
    }

    if(WindowRects[currentNum].hasBeenMoved)
    {
        x = WindowRects[currentNum].movedX;
        y = WindowRects[currentNum].movedY;
    }

    if(CursorInRect(x, y, width, height) && MC::held)
    {
        Logger::debug("yes");
        Logger::debug(std::to_string(currentNum));
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;
    } 
    else if(MC::held && WindowRects[currentNum].isMovingElement)
    {
        Logger::debug("technically yes");
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;
    }

    if(MC::mouseaction == MouseAction::None && !MC::held || MC::mouseaction == MouseAction::Left && !MC::held)
    {
        WindowRects[currentNum].isMovingElement = false;
    }

    RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(D2D1::ColorF::LightGray), width, height);
}
    
void FlarialGUI::UnsetWindowRect()
{
    isInWindowRect = false;
}

Vec2<float> FlarialGUI::CalculateMovedXY(float &x, float &y, int num)
{
    if(isInWindowRect && WindowRects[num].hasBeenMoved)
    {
        x = WindowRects[num].movedX;
        y = WindowRects[num].movedY;
    }

    return Vec2(x, y);
}


std::wstring FlarialGUI::to_wide (const std::string &multi) {
    std::wstring wide; wchar_t w; mbstate_t mb {};
    size_t n = 0, len = multi.length () + 1;
    while (auto res = mbrtowc (&w, multi.c_str () + n, len - n, &mb)) {
        if (res == size_t (-1) || res == size_t (-2))
            throw "invalid encoding";

        n += res;
        wide += w;
    }
    return wide;
}


