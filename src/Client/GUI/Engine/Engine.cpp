#include "Engine.hpp"
#include "Constraints.hpp"
#include "../../Module/Modules/Module.hpp"
#include "../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "animations/fadeinout.hpp"
#include "../../Module/Modules/ClickGUI/GUIMouseListener.hpp"

std::map<std::string, ID2D1Bitmap*> ImagesClass::eimages;

float maxDarkenAmount = 0.1;

bool FlarialGUI::CursorInRect(float rectX, float rectY, float width, float height)
{
    if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height)
    {
        return true;
    }
    return false;
}

static bool CursorInEllipse(float ellipseX, float ellipseY, float radiusX, float radiusY)
{
    float mouseX = MC::mousepos.x;
    float mouseY = MC::mousepos.y;

    float normalizedX = (mouseX - ellipseX) / radiusX;
    float normalizedY = (mouseY - ellipseY) / radiusY;

    return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
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
    D2D1_COLOR_F buttonColor = CursorInRect(x, y, width, height) ? D2D1::ColorF(color.r - darkenAmounts[x+y], color.g - darkenAmounts[x+y], color.b - darkenAmounts[x+y], color.a) : color;
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
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor;

    if (CursorInRect(x, y, width, height))
    {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[x + y], color.g - darkenAmounts[x + y], color.b - darkenAmounts[x + y], color.a);
        FadeEffect::ApplyFadeInEffect(0.005f * FlarialGUI::frameFactor, maxDarkenAmount, darkenAmounts[x+y]);
    } else {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[x + y], color.g - darkenAmounts[x + y], color.b - darkenAmounts[x + y], color.a);
        FadeEffect::ApplyFadeOutEffect(0.005f * FlarialGUI::frameFactor, darkenAmounts[x + y]);

    }

    D2D::context->CreateSolidColorBrush(buttonColor, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    brush->Release();
    writeFactory->Release();
    textFormat->Release();
    textBrush->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;

    }

    return false;
}

bool FlarialGUI::RoundedRadioButton(int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height, float radiusX, float radiusY, const std::string& radioNum, const std::string& currentNum)
{
    if (isInScrollView)
        y += scrollpos;

    static IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));

    static ID2D1SolidColorBrush* textBrush;
    D2D::context->CreateSolidColorBrush(textColor, &textBrush);

    static IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width * 0.64f), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor;

    if(radioNum != currentNum) {
        FadeEffect::ApplyFadeInEffect(0.03f * FlarialGUI::frameFactor, 1, opacityAmounts[index]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[index]);
    }
    else {
        FadeEffect::ApplyFadeOutEffect(0.03f * FlarialGUI::frameFactor, opacityAmounts[index]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[index]);
    }


    D2D::context->CreateSolidColorBrush(buttonColor, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    brush->Release();
    writeFactory->Release();
    textFormat->Release();
    textBrush->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }

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

void FlarialGUI::RoundedHollowRect(float x, float y, float borderWidth, const D2D_COLOR_F color, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    // Calculate the adjusted dimensions for the border rounded rectangle
    D2D1_RECT_F borderRect = D2D1::RectF(
            rect.left - borderWidth / 2.05f,
            rect.top - borderWidth / 2.05f,
            rect.right + borderWidth / 2.05f,
            rect.bottom + borderWidth / 2.05f
    );

    radiusX += borderWidth / 2.05f;
    radiusY += borderWidth / 2.05f;



    D2D::context->DrawRoundedRectangle(D2D1::RoundedRect(borderRect, radiusX, radiusY), brush, borderWidth);
    brush->Release();
}

void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width, float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    ID2D1Factory *factory;
    D2D::context->GetFactory(&factory);

    ID2D1PathGeometry* geometry = nullptr;
    factory->CreatePathGeometry(&geometry);

    ID2D1GeometrySink* sink = nullptr;
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

    ID2D1SolidColorBrush* brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D::context->FillGeometry(geometry, brush);

    Memory::SafeRelease(brush);
    Memory::SafeRelease(sink);
    Memory::SafeRelease(geometry);
    Memory::SafeRelease(factory);
}


bool FlarialGUI::Toggle(int index, float x, float y, const D2D1_COLOR_F enabledColor, const D2D1_COLOR_F disabledColor, const D2D1_COLOR_F circleColor, bool isEnabled) {

    float rectWidth = Constraints::RelativeConstraint(0.062, "height", true);
    float rectHeight = Constraints::RelativeConstraint(0.03, "height", true);

    Vec2<float> round = Constraints::RoundingConstraint(30, 30);

    if(isEnabled) {
        toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index], enabledColor, 0.10f * FlarialGUI::frameFactor);
    } else {
        toggleColors[index] = FlarialGUI::LerpColor(toggleColors[index],  disabledColor, 0.10f * FlarialGUI::frameFactor);
    }

    FlarialGUI::RoundedRect(x, y, toggleColors[index], rectWidth, rectHeight, round.x, round.x);

    // the circle (I KNOW IM USING A RECT LOL)

    float circleWidth = Constraints::RelativeConstraint(0.0202, "height", true);
    float circleHeight = Constraints::RelativeConstraint(0.02, "height", true);


    float ySpacing = Constraints::SpacingConstraint(0.2, circleHeight);
    float xSpacing = Constraints::SpacingConstraint(0.2, circleWidth);
    round = Constraints::RoundingConstraint(23, 23);

    float enabledSpacing;

    if(isEnabled) {
        FadeEffect::ApplyFadeInEffect(2.4 * FlarialGUI::frameFactor, Constraints::SpacingConstraint(1.6, circleWidth), FlarialGUI::toggleSpacings[index]);
        enabledSpacing =  FlarialGUI::toggleSpacings[index];
        if(enabledSpacing > Constraints::SpacingConstraint(1.6, circleWidth)) enabledSpacing = Constraints::SpacingConstraint(1.6, circleWidth);
    } else {
        FadeEffect::ApplyFadeOutEffect(2.4 * FlarialGUI::frameFactor, FlarialGUI::toggleSpacings[index]);
        enabledSpacing =  FlarialGUI::toggleSpacings[index];
    }

    FlarialGUI::RoundedRect(x + xSpacing + enabledSpacing, y + ySpacing, circleColor, circleWidth, circleHeight, round.x, round.x);

    if (CursorInRect(x, y, rectWidth, rectHeight) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }

    return false;
}

D2D_COLOR_F FlarialGUI::LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float t)
{
    // Interpolate each color channel separately
    float r = color1.r + (color2.r - color1.r) * t;
    float g = color1.g + (color2.g - color1.g) * t;
    float b = color1.b + (color2.b - color1.b) * t;
    float a = color1.a + (color2.a - color1.a) * t;

    return D2D1::ColorF(r, g, b, a);
}
void FlarialGUI::ColorWheel(float x, float y, float radius)
{
    // Calculate the center of the color wheel
    D2D1_POINT_2F center = D2D1::Point2F(x - radius / 2.0f, y - radius / 2.0f);

    // Create the color stops for the gradient
    D2D1_GRADIENT_STOP gradientStops[7];

    gradientStops[0].position = 0.0f;   // Red
    gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Red);

    gradientStops[1].position = 1.0f / 6.0f; // Yellow
    gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Yellow);

    gradientStops[2].position = 2.0f / 6.0f; // Green
    gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::Green);

    gradientStops[3].position = 3.0f / 6.0f; // Cyan
    gradientStops[3].color = D2D1::ColorF(D2D1::ColorF::Cyan);

    gradientStops[4].position = 4.0f / 6.0f; // Blue
    gradientStops[4].color = D2D1::ColorF(D2D1::ColorF::Blue);

    gradientStops[5].position = 5.0f / 6.0f; // Magenta
    gradientStops[5].color = D2D1::ColorF(D2D1::ColorF::Magenta);

    gradientStops[6].position = 1.0f;   // Red
    gradientStops[6].color = D2D1::ColorF(D2D1::ColorF::Red);

    // Create the color wheel gradient
    ID2D1GradientStopCollection* gradientStopCollection = nullptr;
    D2D::context->CreateGradientStopCollection(
            gradientStops,
            7,
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            &gradientStopCollection
    );

    // Create the radial gradient brush with appropriate properties
    ID2D1RadialGradientBrush* radialBrush = nullptr;
    D2D::context->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(D2D1::Point2F(x, y), center, radius, radius),
            gradientStopCollection,
            &radialBrush
    );

    // Draw the color wheel
    D2D::context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), radialBrush);

    FlarialGUI::RoundedRect(x, y, D2D1::ColorF(D2D1::ColorF::White), radius, radius);
    FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(D2D1::ColorF::Black), radius, radius);

    // Release resources
    gradientStopCollection->Release();
    radialBrush->Release();
}



void FlarialGUI::HSLToRGB(float h, float s, float l, float& r, float& g, float& b)
{
    if (s == 0.0f)
    {
        r = g = b = l;
    }
    else
    {
        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;

        r = HueToRGB(p, q, h + 1.0f / 3.0f);
        g = HueToRGB(p, q, h);
        b = HueToRGB(p, q, h - 1.0f / 3.0f);
    }
}

float FlarialGUI::HueToRGB(float p, float q, float t)
{
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;

    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

    return p;
}




std::string FlarialGUI::TextBox(int index, float x, float y, float width, float height) {
    FlarialGUI::TextBoxes[index].isActive = true;
    return FlarialGUI::TextBoxes[index].text;
}



float FlarialGUI::Slider(int index, float x, float y, const D2D1_COLOR_F color, const D2D1_COLOR_F disabledColor, const D2D1_COLOR_F circleColor, const float startingPoint, const float maxValue) {

    // Define the total slider rect width and height
    const float totalWidth = Constraints::RelativeConstraint(0.15, "height", true);
    const float height = Constraints::RelativeConstraint(0.0065, "height", true);

    Vec2<float> round = Constraints::RoundingConstraint(12, 12);

    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percWidth = Constraints::RelativeConstraint(0.046, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.029, "height", true);

    FlarialGUI::RoundedRect(x, y, disabledColor, percWidth, percHeight, round.x, round.x);

    FlarialGUI::FlarialText(x - Constraints::SpacingConstraint(0.95, textWidth / 2.0f), y, to_wide(std::to_string((int)startingPoint)).c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth, percHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

    x += Constraints::SpacingConstraint(1.2, percWidth);
    y += Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

    // Calculate the farLeftX and farRightX
    const float farLeftX = x;
    float farRightX = x + totalWidth;

    // Calculate the position of the circle in the middle of the slider rect
    const float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

    float circleX = x + totalWidth / 2.0f;
    float circleY = y + height / 2.0f;

    float rectangleLeft = farLeftX;
    float rectangleWidth = farRightX - farLeftX;

    float minValue = 0.0f;   // Minimum value (e.g., 0%)


    if(SliderRects[index].hasBeenMoved) {


        circleX = (SliderRects[index].percentageX - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

    } else if (startingPoint != 50.0f && !SliderRects[index].hasBeenMoved) {
        circleX = (startingPoint - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;
    }

    // Calculate the position and width of the enabled portion rect
    const float enabledWidth = circleX - farLeftX;

    round = Constraints::RoundingConstraint(6, 6);

    // Draw the disabled portion rect
    RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x,round.x);

    // Draw the enabled portion rect
    RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);
    // Draw the circle in the middle

    FlarialGUI::Circle(circleX, circleY, circleColor, circleRadius);

    // Calculate the percentage
    float percentage = ((circleX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
    SliderRects[index].percentageX = percentage;

    if (CursorInEllipse(circleX, circleY, circleRadius, circleRadius) && MC::held) {
        if(MC::mousepos.x > farLeftX && MC::mousepos.x < farRightX) {

            SliderRects[index].movedX = MC::mousepos.x - circleRadius / 2.0f;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;
        }

        if(MC::mousepos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if(MC::mousepos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;

    } else if (MC::held && SliderRects[index].isMovingElement) {

        if(MC::mousepos.x > farLeftX && MC::mousepos.x < farRightX) {

            SliderRects[index].movedX = MC::mousepos.x - circleRadius / 2.0f;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;
        }

        if(MC::mousepos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if(MC::mousepos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;
    }

    if (MC::mousebutton == MouseButton::None && !MC::held || MC::mousebutton == MouseButton::Left && !MC::held)
    {
        SliderRects[index].isMovingElement = false;
        percentage = SliderRects[index].percentageX;
    }

    return percentage;

}



void FlarialGUI::Circle(float x, float y, const D2D1_COLOR_F& color, float radius) {

    // Create a brush using the specified color
    ID2D1SolidColorBrush* brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    // Create an ellipse with the specified parameters
    D2D1_ELLIPSE ellipse;
    ellipse.point = D2D1::Point2F(x, y);
    ellipse.radiusX = radius;
    ellipse.radiusY = radius;

    // Draw the ellipse using the device context and brush
    D2D::context->FillEllipse(ellipse, brush);

    // Release the brush
    brush->Release();
}





void FlarialGUI::RoundedRectWithImageAndText(int index, float x, float y, const float width, const float height, const D2D1_COLOR_F color, const std::string imagePath, const float imageWidth, const float imageHeight, const wchar_t *text)
{

    float imageY = y;

    if (isInScrollView) {
        y += scrollpos;
        imageY += scrollpos;
    }

    float fakeX = x;
    float fakeY = y;


    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x = x + (width - imageWidth) / 2.0f;
    imageY = imageY + (height - imageHeight) / 2.0f;

    D2D1_RECT_F  imagerectf = D2D1::RectF(x, imageY, x + imageWidth, imageY + imageHeight);

    if (ImagesClass::eimages[imagePath] == nullptr) {

        std::string among = Utils::getRoamingPath() + "\\" + imagePath;
        FlarialGUI::LoadImageFromFile(to_wide(among).c_str(), &ImagesClass::eimages[imagePath]);

    } else if (ImagesClass::eimages[imagePath] != nullptr) {

        D2D1_MATRIX_3X2_F oldTransform;
        D2D::context->GetTransform(&oldTransform);

        if(CursorInRect(x, y, width, height)) {

            FlarialGUI::lerp(rotationAngles[index], rotationAngles[index] + 15, 0.5f * FlarialGUI::frameFactor);
        }


        float rotationAngle = rotationAngles[index];// Specify the rotation angle in degrees
        D2D1_POINT_2F rotationCenter = D2D1::Point2F(imagerectf.left + imageWidth / 2.0f, imagerectf.top +
                                                                                          imageHeight /
                                                                                          2.0f);  // Specify the rotation center
        D2D1_MATRIX_3X2_F rotationMatrix = D2D1::Matrix3x2F::Rotation(rotationAngle, rotationCenter);

        D2D1_MATRIX_3X2_F translationMatrix = D2D1::Matrix3x2F::Translation(x, imageY);
        D2D1_MATRIX_3X2_F combinedMatrix = translationMatrix * rotationMatrix;

        D2D::context->SetTransform(combinedMatrix);

        imagerectf = D2D1::RectF(0, 0, imageWidth, imageHeight);

        D2D::context->DrawBitmap(ImagesClass::eimages[imagePath], imagerectf, 1.0, D2D1_INTERPOLATION_MODE_ANISOTROPIC);

        D2D::context->SetTransform(oldTransform);
    }

    /*
    // Draw text
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);*/

    brush->Release();/*
    textFormat->Release();
    writeFactory->Release();*/
}

void FlarialGUI::ColorPicker(const int index, float x, const float y, std::string &hex) {

    // Accepts hex, so for e.g. fps counter bg color wants to be changed then you'd have to give a modifyable hex value
    // Preferably save every color in config as a hex (string)
    // before rendering just convert the config's color to hex and yeah do it dat way.
    // the Hex parameter in this function should be a pointer directly to the .value of the config
    // change the LoadSettings, see if the config name contains "color" and see if the value is 6 characters long, if so,
    // then load the setting as uint32_t instead of string
    // but when saving, it should be converted to string.

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true);

    FlarialGUI::RoundedRect(x , y + s * 0.15f, D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), s * 4.125f, s, round.x, round.x);

    round = Constraints::RoundingConstraint(10, 10);
    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, FlarialGUI::HexToColorF(hex), s * 0.85f, s * 0.85f, round.x, round.x);

    round = Constraints::RoundingConstraint(11.5, 11.5);
    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f), s * 3.f, s * 0.82f, round.x, round.x);

    FlarialGUI::FlarialTextWithFont(x - Constraints::SpacingConstraint(0.22, s), y * 1.006f, FlarialGUI::to_wide("#" + hex).c_str(), D2D1::ColorF(D2D1::ColorF::White), s * 4.3f, s * 1.1f, DWRITE_TEXT_ALIGNMENT_LEADING, s * 4.5f);

    if (CursorInRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, s * 0.85f, s * 0.85f) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        ColorPickers[index].isActive = !ColorPickers[index].isActive;
    }

    FlarialGUI::ColorPickerWindow(index, hex);
    // rounded rect showcasing this hex, if clicked then ColorPickerWindow

    // text next to rounded rect showing the thing

}

void FlarialGUI::ColorPickerWindow(int index, std::string &hex) {

    if(ColorPickers[index].isActive) {
        // 50% opacity black rect
        FlarialGUI::RoundedRect(0, 0, D2D1::ColorF(D2D1::ColorF::Black, 0.75),
                                Constraints::RelativeConstraint(1.5, "width", true),
                                Constraints::RelativeConstraint(1.5, "height", true), 0, 0);

        // rect containing shit

        // saves this hex to ColorPickers[index].oldHex

        // have 4 sliders for R G B A (converts hex -> d2d1colorf, sets the rgba sliders starting point accordingly)

        // e.g. float r = Slider(...)

        // converts to hex, saves to ColorPickers[index].newHex

        // big rect that show the current hex color (newHex)

        // revert button (reverts to oldHex)

        // save button (converts rgba -> hex, saves to &hex)

    }
}

D2D1::ColorF FlarialGUI::HexToColorF(const std::string& hexString)
{
    // Convert the hex string to an integer value
    uint32_t hex;
    std::stringstream ss;
    ss << std::hex << hexString;
    ss >> hex;

    // Extract the individual color components from the hex value
    uint8_t red = (hex >> 16) & 0xFF;
    uint8_t green = (hex >> 8) & 0xFF;
    uint8_t blue = hex & 0xFF;

    // Normalize the color components to the range [0.0, 1.0]
    float normalizedRed = static_cast<float>(red) / 255.0f;
    float normalizedGreen = static_cast<float>(green) / 255.0f;
    float normalizedBlue = static_cast<float>(blue) / 255.0f;

    // Return the D2D1::ColorF struct
    return {normalizedRed, normalizedGreen, normalizedBlue};
}

std::string FlarialGUI::ColorFToHex(const D2D1::ColorF& color)
{
    // Convert the color components from the range [0.0, 1.0] to [0, 255]
    auto red = static_cast<uint8_t>(std::round(color.r * 255));
    auto green = static_cast<uint8_t>(std::round(color.g * 255));
    auto blue = static_cast<uint8_t>(std::round(color.b * 255));

    // Combine the color components into a hex string
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(red)
       << std::setw(2) << static_cast<int>(green) << std::setw(2) << static_cast<int>(blue);

    return ss.str();
}


void FlarialGUI::FlarialText(float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height, const DWRITE_TEXT_ALIGNMENT alignment)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width), L"", &textFormat);
    textFormat->SetTextAlignment(alignment);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    writeFactory->Release();
    textFormat->Release();
    brush->Release();
}

void FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height, const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(fontSize), L"", &textFormat);
    textFormat->SetTextAlignment(alignment);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    writeFactory->Release();
    textFormat->Release();
    brush->Release();
}

void FlarialGUI::Image(const std::string imageName, D2D1_RECT_F rect)
{
    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    std::string among = Utils::getRoamingPath() + "\\" + imageName;

    if(ImagesClass::eimages[imageName] == nullptr)
    LoadImageFromFile(to_wide(among).c_str(), &ImagesClass::eimages[imageName]);

    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D::context->DrawBitmap(ImagesClass::eimages[imageName], imageRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
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

    if (y - GUIMouseListener::accumilatedBarPos < y + (height * 30.5 / 100))
        whiteY = y - (GUIMouseListener::accumilatedBarPos);
    else
    {
        whiteY = y + (height * 30.5 / 100);
        GUIMouseListener::accumilatedBarPos += barscrollposmodifier;
        GUIMouseListener::accumilatedPos += scrollposmodifier;
    }

    if (y + GUIMouseListener::accumilatedBarPos > y)
    {
        whiteY = y;
        GUIMouseListener::accumilatedBarPos = 0;
        GUIMouseListener::accumilatedPos = 0;
    }

    FlarialGUI::lerp(FlarialGUI::scrollpos, GUIMouseListener::accumilatedPos, 0.30f * FlarialGUI::frameFactor);
    FlarialGUI::lerp(FlarialGUI::barscrollpos, GUIMouseListener::accumilatedBarPos, 0.30f * FlarialGUI::frameFactor);

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
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum)
{
    isInWindowRect = true;

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

void FlarialGUI::ApplyGaussianBlur(float blurIntensity)
{

    // Create Gaussian blur effect
    if(FlarialGUI::blur == nullptr) {

        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);
    }

    if(SwapchainHook::init) {

        ID2D1Bitmap *bitmap = nullptr;

        if(SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        FlarialGUI::blur->SetInput(0, bitmap);

        // Set blur intensity
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurIntensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(FlarialGUI::blur);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(FlarialGUI::blur);
    }
}

void FlarialGUI::CopyBitmap(ID2D1Bitmap1* from, ID2D1Bitmap** to)
{
    if (from == nullptr)
    {
        Logger::debug("from is nullptr");
        return;  // Handle the case where 'from' is nullptr
    }

    if (*to == nullptr)
    {
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr))
        {
            Logger::debug("Failed to create bitmap");
            return;  // Handle the failure to create the bitmap
        }
    }
    else if (from->GetPixelSize() != (*to)->GetPixelSize())
    {
        (*to)->Release();
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr))
        {
            Logger::debug("Failed to create bitmap");
            return;  // Handle the failure to create the bitmap
        }
    }

    D2D1_POINT_2U destPoint = { 0, 0 };
    D2D1_SIZE_U size = from->GetPixelSize();
    D2D1_RECT_U rect = { 0, 0, size.width, size.height };
    (*to)->CopyFromBitmap(&destPoint, from, &rect);
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

template <typename T>
static void FlarialGUI::lerp(T& a, const T& b, float t)
{
    // Perform linear interpolation between a and b based on t
    float interpolatedValue = a + (b - a) * t;

    // Round up the interpolated value to three decimal places
    float roundedValue = std::ceilf(interpolatedValue * 1000.0f) / 1000.0f;

    // Assign the rounded value back to 'a'
    a = roundedValue;
}