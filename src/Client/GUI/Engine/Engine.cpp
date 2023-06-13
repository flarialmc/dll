#include "Engine.hpp"
#include "Constraints.hpp"
#include "../../Module/Modules/Module.hpp"

static bool CursorInRect(float rectX, float rectY, float width, float height)
{
    if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height)
    {
        return true;
    }
    return false;
}

void FlarialGUI::PushSize(float x, float y, float width, float height)
{
    Dimension size;
    size.x = x;
    size.y = y;
    size.width = width;
    size.height = height;

    dimension_stack.push(size);
}

void FlarialGUI::PopSize()
{
    dimension_stack.pop();
}

void FlarialGUI::PopAllStack()
{
    if(!dimension_stack.empty())
    {
        dimension_stack.pop();
    }
}

bool FlarialGUI::Button(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height)
{
    if (isInScrollView)
        y += scrollpos;



    ID2D1SolidColorBrush *brush;
    D2D1_COLOR_F buttonColor = CursorInRect(x, y, width, height) ? D2D1::ColorF(color.r - darkenAmount, color.g - darkenAmount, color.b - darkenAmount, color.a) : color;
    D2D::context->CreateSolidColorBrush(buttonColor, &brush);
    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);


    D2D::context->FillRectangle(rect, brush);
    brush->Release();



    IDWriteTextFormat *textFormat;
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormat); ID2D1SolidColorBrush *textBrush; D2D::context->CreateSolidColorBrush(textColor, &textBrush); textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush); textBrush->Release(); textFormat->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }
    else
    {
        MC::mousebutton = MouseButton::None;
        return false;
    }
}


            
bool FlarialGUI::RoundedButton(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    static IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));

    static ID2D1SolidColorBrush* textBrush;
    D2D::context->CreateSolidColorBrush(textColor, &textBrush);

    static IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width, height), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    const float darkenAmount = 0.1f;

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }



    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor = color;
    if (CursorInRect(x, y, width, height))
    {
        buttonColor = D2D1::ColorF(color.r - darkenAmount, color.g - darkenAmount, color.b - darkenAmount, color.a);
    }
    D2D::context->CreateSolidColorBrush(buttonColor, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    brush->Release();
    writeFactory->Release();
    textFormat->Release();
    textBrush->Release();
    return false;
}

void FlarialGUI::RoundedRect(float x, float y, const D2D_COLOR_F color, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);

    D2D::context->FillRoundedRectangle(roundedRect, brush);
    brush->Release();
}

void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width, float height, float radiusX, float radiusY)
{

    if (isInScrollView)
        y += scrollpos;
    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    ID2D1Factory *factory;
    D2D::context->GetFactory(&factory);

    ID2D1PathGeometry *geometry = nullptr;
    factory->CreatePathGeometry(&geometry);

    ID2D1GeometrySink *sink = nullptr;
    geometry->Open(&sink);

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

    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D::context->FillGeometry(geometry, brush);

    factory->Release();
    sink->Release();
    geometry->Release();
}

void FlarialGUI::RoundedRectWithImageAndText(float x, float y, const float width, const float height, const D2D1_COLOR_F color, const std::string imagePath, const float imageWidth, const float imageHeight, const wchar_t *text)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x = x + (width - imageWidth) / 2.0f;
    y = y + (height - imageHeight) / 2.0f;

    D2D1_RECT_F  imagerect = D2D1::RectF(x, y, x + imageWidth, y + imageHeight);

    Image(imagePath, imagerect);

    // Draw text
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    brush->Release();
    textFormat->Release();
    writeFactory->Release();
}

void FlarialGUI::Image(const std::string imageName, D2D1_RECT_F rect)
{
    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    ID2D1Bitmap *bitmap;
    std::string among = Utils::getRoamingPath() + "\\" + imageName;
    LoadImageFromFile(to_wide(among).c_str(), &bitmap);

    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D::context->DrawBitmap(bitmap, imageRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    bitmap->Release();
}

void FlarialGUI::FlarialText(float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width, height), L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    writeFactory->Release();
    textFormat->Release();
    brush->Release();
}

void FlarialGUI::LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap)
{
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    IWICBitmapDecoder *bitmapDecoder = nullptr;
    IWICBitmapFrameDecode *frame = nullptr;
    IWICFormatConverter *formatConverter = nullptr;

    IWICImagingFactory *imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

    // Create decoder
    imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);

    // Get first frame
    bitmapDecoder->GetFrame(0, &frame);

    // Convert format to 32bppPBGRA
    imagingFactory->CreateFormatConverter(&formatConverter);
    formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);

    // Create bitmap
    D2D::context->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);

    bitmapDecoder->Release();
    frame->Release();
    formatConverter->Release();
    imagingFactory->Release();
}

void FlarialGUI::SetScrollView(float x, float y, float width, float height)
{
    FlarialGUI::isInScrollView = true;
    D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
    D2D::context->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void FlarialGUI::UnsetScrollView()
{
    FlarialGUI::isInScrollView = false;
    D2D::context->PopAxisAlignedClip();
}

void FlarialGUI::ScrollBar(float x, float y, float width, float height, float radius)
{

    float whiteY;

    if (y - barscrollpos < y + (height * 30.5 / 100))
        whiteY = y - (barscrollpos);
    else
    {
        whiteY = y + (height * 30.5 / 100);
        barscrollpos += barscrollposmodifier;
        scrollpos += scrollposmodifier;
    }

    if (y + barscrollpos > y)
    {
        whiteY = y;
        barscrollpos = 0;
        scrollpos = 0;
    }

    // Draw the gray bar
    ID2D1SolidColorBrush *graybrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &graybrush);
    D2D1_ROUNDED_RECT grayRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radius, radius);
    D2D::context->FillRoundedRectangle(&grayRect, graybrush);
    graybrush->Release();
    // Draw the white bar
    ID2D1SolidColorBrush *whitebrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whitebrush);
    D2D1_ROUNDED_RECT whiteRect = D2D1::RoundedRect(D2D1::RectF(x, whiteY, x + width, whiteY + (height * 69.5 / 100)), radius, radius);
    D2D::context->FillRoundedRectangle(&whiteRect, whitebrush);
    whitebrush->Release();
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum)
{
    isInWindowRect = true;
    if (!WindowRects[currentNum].madeRect)
    {
        WindowRects[currentNum] = WindowRect();
        WindowRects[currentNum].madeRect = true;
    }

    if (WindowRects[currentNum].hasBeenMoved)
    {
        x = Constraints::PercentageConstraint(WindowRects[currentNum].percentageX, "left");
        y = Constraints::PercentageConstraint(WindowRects[currentNum].percentageY, "top");
    }

    if (CursorInRect(x, y, width, height) && MC::held)
    {
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;
        WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
        WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;
    }
    else if (MC::held && WindowRects[currentNum].isMovingElement)
    {
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;

        WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
        WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;
    }

    if (MC::mousebutton == MouseButton::None && !MC::held || MC::mousebutton == MouseButton::Left && !MC::held)
    {
        WindowRects[currentNum].isMovingElement = false;
    }

    RoundedRectOnlyTopCorner(x, y, D2D1::ColorF(D2D1::ColorF::LightGray, 0.0), width, height);
}

void FlarialGUI::UnsetWindowRect()
{
    isInWindowRect = false;
}

void FlarialGUI::UpdateWindowRects()
{
    for (int i = 0; i < static_cast<int>(sizeof WindowRects); i++)
    {
        WindowRect &rect = WindowRects[i];
        if (rect.hasBeenMoved)
        {
            rect.movedX = Constraints::PercentageConstraint(rect.percentageX, "left");
            rect.movedY = Constraints::PercentageConstraint(rect.percentageY, "top");
        }
        else
        {
            return;
        }
    }
}

Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num)
{
    if (isInWindowRect && WindowRects[num].hasBeenMoved)
    {
        x = Constraints::PercentageConstraint(WindowRects[num].percentageX, "left");
        y = Constraints::PercentageConstraint(WindowRects[num].percentageY, "top");
    }

    return {x, y};
}

Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight)
{
    Vec2<float> xy;
    xy.x = (D2D::context->GetSize().width - rectWidth) / 2.0f;
    xy.y = (D2D::context->GetSize().height - rectHeight) / 2.0f;
    return xy;
}

Vec2<float> FlarialGUI::CalculateResizedXY(float x, float y, float width, float height)
{
    if (MC::hasResized)
    {
        x = (MC::windowSize.x - width) / 2;
        y = (MC::windowSize.y - height) / 2;
    }

    return Vec2(x, y);
}

std::wstring FlarialGUI::to_wide(const std::string &multi)
{
    std::wstring wide;
    wchar_t w;
    mbstate_t mb{};
    size_t n = 0, len = multi.length() + 1;
    while (auto res = mbrtowc(&w, multi.c_str() + n, len - n, &mb))
    {
        if (res == size_t(-1) || res == size_t(-2))
            throw "invalid encoding";

        n += res;
        wide += w;
    }
    return wide;
}