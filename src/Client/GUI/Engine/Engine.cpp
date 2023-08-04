#include "Engine.hpp"
#include "../../Client.hpp"
#include <utility>
#include "Constraints.hpp"
#include "animations/fadeinout.hpp"
#include "../../Module/Modules/ClickGUI/GUIMouseListener.hpp"

std::map<std::string, ID2D1Bitmap*> ImagesClass::eimages;
IDWriteFactory *FlarialGUI::writeFactory;
ID2D1ImageBrush* FlarialGUI::blurbrush;
ID2D1ImageBrush* FlarialGUI::shadowbrush;
ID2D1Factory* FlarialGUI::factory;
std::unordered_map<std::string, ID2D1SolidColorBrush*> FlarialGUI::brushCache;
std::unordered_map<std::string, ID2D1Image*> FlarialGUI::cachedBitmaps;

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
    brush = FlarialGUI::getBrush(buttonColor);
    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);


    D2D::context->FillRectangle(rect, brush);


    IDWriteTextFormat *textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormat);
    ID2D1SolidColorBrush *textBrush;
    textBrush = FlarialGUI::getBrush(textColor);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat,
    D2D1::RectF(x, y, x + width, y + height), textBrush);

     textFormat->Release();

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



bool FlarialGUI::RoundedButton(const int index, float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    static ID2D1SolidColorBrush* textBrush;
    textBrush = FlarialGUI::getBrush(textColor);

    static IDWriteTextFormat* textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor;

    if (CursorInRect(x, y, width, height))
    {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index], color.b - darkenAmounts[index], color.a);
        FadeEffect::ApplyFadeInEffect(0.005f * FlarialGUI::frameFactor, maxDarkenAmount, darkenAmounts[index]);
    } else {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[index], color.g - darkenAmounts[index], color.b - darkenAmounts[index], color.a);
        FadeEffect::ApplyFadeOutEffect(0.005f * FlarialGUI::frameFactor, darkenAmounts[index]);

    }


    brush = FlarialGUI::getBrush(buttonColor);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    textFormat->Release();

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


    static ID2D1SolidColorBrush* textBrush;

    textBrush = FlarialGUI::getBrush(textColor);

    static IDWriteTextFormat* textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width * 0.84f), L"en-US", &textFormat);
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


    brush = FlarialGUI::getBrush(buttonColor);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x += Constraints::SpacingConstraint(0.077, width);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    textFormat->Release();

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
    brush = FlarialGUI::getBrush(color);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);

    D2D::context->FillRoundedRectangle(roundedRect, brush);
}

void FlarialGUI::RoundedHollowRect(float x, float y, float borderWidth, const D2D_COLOR_F color, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    ID2D1SolidColorBrush *brush;
    brush = FlarialGUI::getBrush(color);
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    // Make the border extend from the outside only
    D2D1_RECT_F borderRect = D2D1::RectF(
            rect.left - borderWidth / 1.80f,
            rect.top - borderWidth / 1.80f,
            rect.right + borderWidth / 1.80f,
            rect.bottom + borderWidth / 1.80f
    );

    radiusX += borderWidth / 1.80f;
    radiusY += borderWidth / 1.80f;



    D2D::context->DrawRoundedRectangle(D2D1::RoundedRect(borderRect, radiusX, radiusY), brush, borderWidth);
}

void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width, float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    if(factory == nullptr)
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

    brush = FlarialGUI::getBrush(color);

    D2D::context->FillGeometry(geometry, brush);

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

    if(isInScrollView) y += FlarialGUI::scrollpos;
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


std::string FlarialGUI::TextBoxVisual(int index, std::string& text, int limit, float x, float y, std::string real) {

    D2D1_COLOR_F col;

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);

    text = FlarialGUI::TextBox(index, text, 16, x, y, Constraints::SpacingConstraint(1.55, textWidth), percHeight);

    if(TextBoxes[index].isActive) col = D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f);
    else col = D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f);


    FlarialGUI::RoundedRect(x, y, col, Constraints::SpacingConstraint(1.55, textWidth), percHeight, round.x, round.x);

    FlarialGUI::FlarialTextWithFont(x, y, to_wide(text).c_str(), D2D1::ColorF(D2D1::ColorF::White), Constraints::SpacingConstraint(1.55, textWidth), percHeight, DWRITE_TEXT_ALIGNMENT_CENTER, Constraints::SpacingConstraint(1.0, textWidth));
    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.70, textWidth), y, to_wide(real).c_str(), D2D1::ColorF(D2D1::ColorF::White), Constraints::SpacingConstraint(3, textWidth), percHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.00, textWidth));

    return "";
}


std::string FlarialGUI::TextBox(int index, std::string text, int limit, float x, float y, float width, float height) {

    if(isInScrollView) y += scrollpos;

    if(CursorInRect(x, y, width, height) && MC::mouseaction == MouseAction::PRESS && MC::mousebutton == MouseButton::Left) {

        FlarialGUI::TextBoxes[index].isActive = true;

    } else if (!CursorInRect(x, y, width, height) && MC::mouseaction == MouseAction::PRESS && MC::mousebutton == MouseButton::Left) {

        FlarialGUI::TextBoxes[index].isActive = false;
        FlarialGUI::TextBoxes[index].text = text;

    } else if (!FlarialGUI::TextBoxes[index].isActive) {

        FlarialGUI::TextBoxes[index].text = text;
    }


    if(FlarialGUI::TextBoxes[index].text.empty() && FlarialGUI::TextBoxes[index].firstTime) {
        FlarialGUI::TextBoxes[index].firstTime = false;
        FlarialGUI::TextBoxes[index].text = text;
    }

    FlarialGUI::TextBoxes[index].text = FlarialGUI::TextBoxes[index].text.substr(0, limit);

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

    std::string text;

    if(startingPoint < 10.0f) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << startingPoint;
        text = stream.str();
    }
    else text = std::to_string((int)startingPoint);

    FlarialGUI::FlarialText(x - Constraints::SpacingConstraint(0.62, textWidth / 2.0f), y, to_wide(text).c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth, percHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

    x += Constraints::SpacingConstraint(1.2, percWidth);
    y += Constraints::SpacingConstraint(0.8, percHeight / 2.0f);

    // Calculate the farLeftX and farRightX
    const float farLeftX = x;
    float farRightX = x + totalWidth;

    // Calculate the position of the circle in the middle of the slider rect
    const float circleRadius = Constraints::RelativeConstraint(0.008, "height", true);

    float circleX = x + totalWidth / 2.0f;

    float circleY;

    if(FlarialGUI::isInScrollView)
    circleY = (y + scrollpos) + height / 2.0f;
    else  circleY = y + height / 2.0f;

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

            SliderRects[index].movedX = MC::mousepos.x;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;
        }

        if(MC::mousepos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if(MC::mousepos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;

    } else if (MC::held && SliderRects[index].isMovingElement) {

        if(MC::mousepos.x > farLeftX && MC::mousepos.x < farRightX) {

            SliderRects[index].movedX = MC::mousepos.x;
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

    if(percentage < 0.02) percentage = 0.01;

    return percentage;

}



void FlarialGUI::Circle(float x, float y, const D2D1_COLOR_F& color, float radius) {

    // Create a brush using the specified color
    ID2D1SolidColorBrush* brush;

    brush = FlarialGUI::getBrush(color);

    // Create an ellipse with the specified parameters
    D2D1_ELLIPSE ellipse;
    ellipse.point = D2D1::Point2F(x, y);
    ellipse.radiusX = radius;
    ellipse.radiusY = radius;

    // Draw the ellipse using the device context and brush
    D2D::context->FillEllipse(ellipse, brush);
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

    brush = FlarialGUI::getBrush(color);

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
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);*/

    /*
    textFormat->Release();
    FlarialGUI::writeFactory->Release();*/
}

void FlarialGUI::KeybindSelector(const int index, float x, float y, std::string &keybind) {

    Vec2<float> round = Constraints::RoundingConstraint(13, 13);

    float s = Constraints::RelativeConstraint(0.0285, "height", true);

    const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
    const float percWidth = Constraints::RelativeConstraint(0.069, "height", true);
    const float percHeight = Constraints::RelativeConstraint(0.035, "height", true);


    D2D1_COLOR_F col;

    if(KeybindSelectors[index].isActive) {
        col = D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f);

        std::chrono::steady_clock::time_point currentOnKeyTime = std::chrono::steady_clock::now();
        auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(currentOnKeyTime - KeybindSelectors[index].currentOnKeyTime);

        if(timeDifference.count() > 2000) KeybindSelectors[index].isActive = false;
    }

    else col = D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f);

    FlarialGUI::RoundedRect(x, y, col, percWidth, percHeight, round.x, round.x);

    std::string text;
    if(KeybindSelectors[index].isActive) {

        if(FlarialGUI::currentKeybind != "nothing") {

            KeybindSelectors[index].oldShi = keybind;
            keybind = FlarialGUI::currentKeybind;
            KeybindSelectors[index].newShi = keybind;

        } else {
            FlarialGUI::currentKeybind = "";
            keybind = "";
        }
    }

    text = keybind;

    FlarialGUI::FlarialText(x - Constraints::SpacingConstraint(0.42, textWidth / 2.0f), y, to_wide(text).c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth, percHeight, DWRITE_TEXT_ALIGNMENT_CENTER);

    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.25, textWidth / 2.0f), y, to_wide("Keybind (Hold for 2 seconds)").c_str(), D2D1::ColorF(D2D1::ColorF::White), Constraints::SpacingConstraint(2.2, textWidth), percHeight, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.0, textWidth));

    if(isInScrollView) y += scrollpos;

    if (CursorInRect(x, y, percWidth, percHeight) && MC::mousebutton == MouseButton::Left && !MC::held && !KeybindSelectors[index].isActive)
    {
        KeybindSelectors[index].isActive = !KeybindSelectors[index].isActive;
        KeybindSelectors[index].currentOnKeyTime = std::chrono::steady_clock::now();
    }

    if (!CursorInRect(x, y, percWidth, percHeight) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        KeybindSelectors[index].isActive = false;
    }

}

void FlarialGUI::ColorPicker(const int index, float x, const float y, std::string &hex, float &opacity) {

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

    D2D1_COLOR_F color = FlarialGUI::HexToColorF(hex);
    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, color, s * 0.85f, s * 0.85f, round.x, round.x);

    round = Constraints::RoundingConstraint(11.5, 11.5);

    D2D1_COLOR_F col = D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f);

    if(TextBoxes[index].isActive) col = D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f);

    FlarialGUI::RoundedRect(x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, col, s * 3.f, s * 0.82f, round.x, round.x);

    std::string text;
    hex = FlarialGUI::TextBox(index, hex, 6, x + Constraints::SpacingConstraint(1.05, s), y + s * 0.23f, s * 3.f, s * 0.82f);

    text = "#" + hex;

    FlarialGUI::FlarialTextWithFont(x + Constraints::SpacingConstraint(1.35f, s), y * 1.006f, FlarialGUI::to_wide(text).c_str(), D2D1::ColorF(D2D1::ColorF::White), s * 4.3f, s * 1.1f, DWRITE_TEXT_ALIGNMENT_LEADING, s * 4.0f);

    if (CursorInRect(x + Constraints::SpacingConstraint(0.1, s), y + s * 0.21f, s * 0.85f, s * 0.85f) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        ColorPickers[index].isActive = !ColorPickers[index].isActive;
    }

}

void FlarialGUI::ColorPickerWindow(int index, std::string &hex, float& opacity) {

    int i = 1;
    for (auto picker : ColorPickers)
        if(i != index && picker.isActive) picker.isActive = false;

    if(ColorPickers[index].isActive) {


        // 50% opacity black rect
        FlarialGUI::RoundedRect(0, 0, D2D1::ColorF(D2D1::ColorF::Black, 0.75),
                                Constraints::RelativeConstraint(1.5, "width", true),
                                Constraints::RelativeConstraint(1.5, "height", true), 0, 0);

        D2D1_COLOR_F color = FlarialGUI::HexToColorF(hex);

        float rectwidth = Constraints::RelativeConstraint(0.35, "height", true);
        float rectheight = Constraints::RelativeConstraint(0.25, "height", true);
        Vec2<float> center = Constraints::CenterConstraint(rectwidth, rectheight);
        Vec2<float> round = Constraints::RoundingConstraint(45, 45);

        FlarialGUI::SetWindowRect(center.x, center.y - Constraints::RelativeConstraint(0.03, "height", true), rectwidth, Constraints::RelativeConstraint(0.03, "height", true), index + 50);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(center.x, center.y, index + 50);
        center.x = vec2.x;
        center.y = vec2.y;

        FlarialGUI::RoundedHollowRect(center.x, center.y, Constraints::RelativeConstraint(0.01, "height", true), D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), rectwidth, rectheight, round.x, round.x);
        FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), rectwidth, rectheight, round.x, round.x);

        FlarialGUI::PushSize(center.x, center.y, rectwidth, rectheight);

        float x = Constraints::PercentageConstraint(0.18, "left");
        float y = Constraints::PercentageConstraint(0.20, "top");
        float spacing = Constraints::SpacingConstraint(0.15, rectheight);

        FlarialGUI::FlarialTextWithFont(x + spacing, y - spacing * 1.45, L"Color Sliders", D2D1::ColorF(D2D1::ColorF::White), Constraints::RelativeConstraint(1, "width"), Constraints::RelativeConstraint(0.2, "width"), DWRITE_TEXT_ALIGNMENT_JUSTIFIED, Constraints::RelativeConstraint(0.5, "width"));

        spacing *= 0.90f;

        float percentR = FlarialGUI::Slider(index + 10, x, y,
                                            D2D1::ColorF(D2D1::ColorF::Red),
                                            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                            D2D1::ColorF(D2D1::ColorF::White), color.r * 255.0f, 255.0f);

        float percentG = FlarialGUI::Slider(index + 11, x, y + spacing,
                                            D2D1::ColorF(D2D1::ColorF::Green),
                                            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                            D2D1::ColorF(D2D1::ColorF::White), color.g * 255.0f, 255.0f);

        float percentB = FlarialGUI::Slider(index + 12, x, y + spacing * 2,
                                            D2D1::ColorF(D2D1::ColorF::Blue),
                                            D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f),
                                            D2D1::ColorF(D2D1::ColorF::White), color.b * 255.0f, 255.0f);

        opacity = FlarialGUI::Slider(index + 200, x, y + spacing * 3,
                                     D2D1::ColorF(255.0f / 255.0f, 36.0f / 255.0f, 56.0f / 255.0f, opacity),
                                     D2D1::ColorF(154.0f / 255.0f, 107.0f / 255.0f, 114.0f / 255.0f, opacity),
                                     D2D1::ColorF(D2D1::ColorF::White), opacity, 1.0);

        float buttonWidth = Constraints::RelativeConstraint(0.35f, "width");
        float buttonHeight = Constraints::RelativeConstraint(0.20f, "height");
        if(FlarialGUI::RoundedButton(0, x + spacing * 1.45f, y + spacing * 4.12f, D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), D2D1::ColorF(D2D1::ColorF::White), L"Close", buttonWidth, buttonHeight, round.x, round.x)) ColorPickers[index].isActive = false;



        color.r = percentR / 255.0f;
        color.g = percentG / 255.0f;
        color.b = percentB / 255.0f;

        hex = FlarialGUI::ColorFToHex(color);

        FlarialGUI::PopSize();

        FlarialGUI::UnsetWindowRect();

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
    if (hexString.length() != 6)
    {
        // Return black if the string length is not 6
        return D2D1::ColorF(0.0f, 0.0f, 0.0f);
    }

    try
    {
        // Convert the hex string to individual color components
        uint32_t hex = std::stoul(hexString, nullptr, 16);
        uint8_t red = (hex >> 16) & 0xFF;
        uint8_t green = (hex >> 8) & 0xFF;
        uint8_t blue = hex & 0xFF;

        // Normalize the color components to the range [0.0, 1.0]
        float normalizedRed = static_cast<float>(red) / 255.0f;
        float normalizedGreen = static_cast<float>(green) / 255.0f;
        float normalizedBlue = static_cast<float>(blue) / 255.0f;

        // Return the D2D1::ColorF struct
        return D2D1::ColorF(normalizedRed, normalizedGreen, normalizedBlue);
    }
    catch (const std::exception&)
    {
        // Return black if the conversion fails
        return D2D1::ColorF(0.0f, 0.0f, 0.0f);
    }
}



std::string FlarialGUI::ColorFToHex(const D2D1_COLOR_F& color)
{
    // Convert the color components from the range [0.0, 1.0] to [0, 255]
    uint8_t red = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t green = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t blue = static_cast<uint8_t>(color.b * 255.0f);

    // Combine the color components into a hex string
    char hexString[7];
    sprintf(hexString, "%02x%02x%02x", red, green, blue);

    return std::string(hexString);
}


void FlarialGUI::FlarialText(float x, float y, const wchar_t *text, D2D1_COLOR_F color, float width, const float height, const DWRITE_TEXT_ALIGNMENT alignment)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;

    brush = FlarialGUI::getBrush(color);

    IDWriteTextFormat *textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width), L"", &textFormat);
    textFormat->SetTextAlignment(alignment);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    textFormat->Release();

}

void FlarialGUI::FlarialTextWithFont(float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height, const DWRITE_TEXT_ALIGNMENT alignment, const float fontSize, const DWRITE_FONT_WEIGHT weight)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;

    brush = FlarialGUI::getBrush(color);


    IDWriteTextFormat *textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(fontSize), L"", &textFormat);
    textFormat->SetTextAlignment(alignment);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    textFormat->Release();
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

    if (y - GUIMouseListener::accumilatedPos > y + height) {
        GUIMouseListener::accumilatedPos += scrollposmodifier;
    }

    if (y + GUIMouseListener::accumilatedPos > y)
    {
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

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum)
{
    isInWindowRect = true;

    int i = 0;
    bool ye = false;
    for(auto& rect : WindowRects) {

        if(rect.isMovingElement && i != currentNum) {
            ye = true;
            break;
        }

        i++;
    }

    if(!ye) {

        if (WindowRects[currentNum].hasBeenMoved) {
            x = Constraints::PercentageConstraint(WindowRects[currentNum].percentageX, "left", true);
            y = Constraints::PercentageConstraint(WindowRects[currentNum].percentageY, "top", true);
        }

        if (CursorInRect(x, y, width, height) && MC::held) {
            WindowRects[currentNum].isMovingElement = true;
            WindowRects[currentNum].hasBeenMoved = true;
            WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
            WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;
            WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
            WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;
        } else if (MC::held && WindowRects[currentNum].isMovingElement) {
            WindowRects[currentNum].isMovingElement = true;
            WindowRects[currentNum].hasBeenMoved = true;
            WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
            WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;

            WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
            WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;

        }

        if (MC::mousebutton == MouseButton::None && !MC::held || MC::mousebutton == MouseButton::Left && !MC::held) {
            WindowRects[currentNum].isMovingElement = false;
        }
    }
}

void FlarialGUI::UnsetWindowRect()
{
    isInWindowRect = false;
}

Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num, float rectWidth, float rectHeight)
{
    if (isInWindowRect && WindowRects[num].hasBeenMoved)
    {
        x = (WindowRects[num].percentageX * MC::windowSize.x);
        y = (WindowRects[num].percentageY * MC::windowSize.y);
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


void FlarialGUI::ResetShit() {

    // Reset the variables to their initial values or desired values here
    for (auto & i : WindowRects) {
        i = WindowRect();
    }

    for (auto & i : SliderRects) {
        i = SliderRect();
    }

    for (auto & TextBoxe : TextBoxes) {
        TextBoxe = TextBoxStruct();
    }

    for (auto & i : ColorPickers) {
        i = ::ColorPicker();
    }


}

void FlarialGUI::ApplyCombinedDeepFry()
{

    if (SwapchainHook::init) {

        ID2D1Effect* cum = nullptr;
        ID2D1Effect* cum2 = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, &cum);
        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, &cum2);

        ID2D1Bitmap* bitmap = nullptr;

        if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        float table[3] = { 0.0f, 0.5f, 1.0f };
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_RED_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_GREEN_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_BLUE_TABLE, table);

        ID2D1Image* bitmap2;
        cum->GetOutput(&bitmap2);
        cum2->SetInput(0, bitmap2);

        cum2->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum2);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(bitmap2);
        Memory::SafeRelease(cum);
        Memory::SafeRelease(cum2);
    }
}

void FlarialGUI::ApplyAestheticDeepFry()
{

    if (SwapchainHook::init) {

        ID2D1Effect* cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, &cum);

        ID2D1Bitmap* bitmap = nullptr;

        if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        float table[3] = { 0.0f, 0.5f, 1.0f };
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_RED_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_GREEN_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_BLUE_TABLE, table);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(cum);
    }
}

void FlarialGUI::ApplyDeepFry(float intensity)
{

    if (SwapchainHook::init) {

        ID2D1Effect* cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, &cum);

        ID2D1Bitmap* bitmap = nullptr;

        if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        cum->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, intensity);
        cum->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        cum->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        cum->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        cum->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(cum);
    }
}

void FlarialGUI::ApplyHue(float Hue)
{

    if (SwapchainHook::init) {

        ID2D1Effect* cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1Saturation, &cum);

        ID2D1Bitmap* bitmap = nullptr;

        if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        float table[3] = { 0.0f, 0.5f, 1.0f };
        cum->SetValue(D2D1_SATURATION_PROP_SATURATION, Hue);
        D2D::context->DrawImage(cum);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(cum);
    }
}

void FlarialGUI::ApplyPaintEffect(float intensity)
{

    if (SwapchainHook::init) {

        ID2D1Effect* cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1Morphology, &cum);

        ID2D1Bitmap* bitmap = nullptr;

        if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        cum->SetValue(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_ERODE);
        cum->SetValue(D2D1_MORPHOLOGY_PROP_WIDTH, intensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(cum);
    }
}


void FlarialGUI::ApplyGaussianBlur(float blurIntensity)
{
    FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y), 0, 0), 0.0);
}

void FlarialGUI::ApplySusGaussianBlur(float blurIntensity)
{

    ID2D1Effect* effect;
    D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &effect);

    if(SwapchainHook::init) {

        ID2D1Image* input;
        D2D::context->GetTarget(&input);

        effect->SetInput(0, input);

        // Set blur intensity
        effect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurIntensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(effect);

        Memory::SafeRelease(input);
        Memory::SafeRelease(effect);
    }
}

void FlarialGUI::Notify(std::string text) {

    Notification e;
    e.text = text;
    e.finished = false;
    e.currentPos = Constraints::PercentageConstraint(0.01, "right", true);
    e.currentPosY = Constraints::PercentageConstraint(0.25, "bottom", true);

    IDWriteTextFormat *textFormat;
    FlarialGUI::writeFactory->CreateTextFormat(FlarialGUI::to_wide(Client::settings.getSettingByName<std::string>("fontname")->value).c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(Constraints::SpacingConstraint(0.3, Constraints::RelativeConstraint(0.45, "height", true))), L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    IDWriteTextLayout *textLayout;

    FlarialGUI::writeFactory->CreateTextLayout(
            FlarialGUI::to_wide(text).c_str(),
            wcslen(FlarialGUI::to_wide(text).c_str()),
            textFormat,
            Constraints::RelativeConstraint(0.45, "height", true),
            Constraints::RelativeConstraint(0.10, "height", true),
            &textLayout
    );

    DWRITE_TEXT_METRICS textMetrics;
    textLayout->GetMetrics(&textMetrics);

    e.width = textMetrics.width + Constraints::SpacingConstraint(0.65f, textMetrics.width);

    notifications.push_back(e);

}

void FlarialGUI::NotifyHeartbeat() {


    float rectHeight = Constraints::RelativeConstraint(0.10, "height", true);

    float y = Constraints::PercentageConstraint(0.25, "bottom", true);

    Vec2<float> rounding = Constraints::RoundingConstraint(20, 20);

    int i = 0;

    for (auto& notif : FlarialGUI::notifications) {

        float rectWidth = notif.width;
        float x = Constraints::PercentageConstraint(0.01, "right", true) - Constraints::SpacingConstraint(0.85, rectWidth);

        FlarialGUI::lerp(notif.currentPosY, y, 0.20f * FlarialGUI::frameFactor);

        if(!notif.arrived) {

            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(
                    D2D1::RectF(notif.currentPos, notif.currentPosY, notif.currentPos + rectWidth, notif.currentPosY + rectHeight), rounding.x,
                    rounding.x);

            D2D1_COLOR_F col = FlarialGUI::HexToColorF("110F10");
            col.a = 0.60;

            FlarialGUI::BlurRect(rect, 6.0f);
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY,
                                    col, rectWidth,
                                    rectHeight, rounding.x, rounding.x);

            D2D1_RECT_F cutoutrect = D2D1::RectF(notif.currentPos, notif.currentPosY, notif.currentPos + Constraints::SpacingConstraint(0.0127, Constraints::RelativeConstraint(0.45, "height", true)), notif.currentPosY + rectHeight);

            D2D::context->PushAxisAlignedClip(cutoutrect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            col = FlarialGUI::HexToColorF("FE2438");
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY,
                                    col, rectWidth,
                                    rectHeight, rounding.x, rounding.x);

            D2D::context->PopAxisAlignedClip();

            FlarialGUI::PushSize(notif.currentPos, notif.currentPosY, rectWidth, rectHeight);

            float logoX = Constraints::PercentageConstraint(0.01, "left") - Constraints::SpacingConstraint(0.18, rectHeight);
            float logoY = Constraints::PercentageConstraint(0.01, "top") - Constraints::SpacingConstraint(0.10, rectHeight);
            float logoWidth = Constraints::RelativeConstraint(1.25);

            FlarialGUI::Image("\\Flarial\\assets\\logo.png", D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            logoX += Constraints::SpacingConstraint(0.85, logoWidth);
            logoY -= Constraints::SpacingConstraint(0.105, logoWidth);
            FlarialGUI::FlarialTextWithFont(logoX, logoY, FlarialGUI::to_wide("Notification").c_str(), D2D1::ColorF(D2D1::ColorF::White), rectWidth, logoWidth, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.45, Constraints::RelativeConstraint(0.45, "height", true)), DWRITE_FONT_WEIGHT_BOLD);

            logoY += Constraints::SpacingConstraint(0.185, logoWidth);
            FlarialGUI::FlarialTextWithFont(logoX, logoY, FlarialGUI::to_wide(notif.text).c_str(), D2D1::ColorF(D2D1::ColorF::White), rectWidth, logoWidth, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.3, Constraints::RelativeConstraint(0.45, "height", true)));


            FlarialGUI::PopSize();

            FlarialGUI::lerp(notif.currentPos, x - 3, 0.12f * FlarialGUI::frameFactor);

            if(notif.currentPos < x || notif.currentPos == x) {
                notif.arrived = true;
                notif.time = std::chrono::steady_clock::now();
            }

            i++;

        } else {

            std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
            auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(current - notif.time);

            if(timeDifference.count() > 5000) {

                FlarialGUI::lerp(notif.currentPos, Constraints::PercentageConstraint(0.01, "right", true) + 50, 0.12f * FlarialGUI::frameFactor);

            }

            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(
                    D2D1::RectF(notif.currentPos, notif.currentPosY, notif.currentPos + rectWidth, notif.currentPosY + rectHeight), rounding.x,
                    rounding.x);

            D2D1_COLOR_F col = FlarialGUI::HexToColorF("110F10");
            col.a = 0.60;

            FlarialGUI::BlurRect(rect, 6.0f);
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY,
                                    col, rectWidth,
                                    rectHeight, rounding.x, rounding.x);

            D2D1_RECT_F cutoutrect = D2D1::RectF(notif.currentPos, notif.currentPosY, notif.currentPos + Constraints::SpacingConstraint(0.0127, Constraints::RelativeConstraint(0.45, "height", true)), notif.currentPosY + rectHeight);

            D2D::context->PushAxisAlignedClip(cutoutrect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            col = FlarialGUI::HexToColorF("FE2438");
            FlarialGUI::RoundedRect(notif.currentPos, notif.currentPosY,
                                    col, rectWidth,
                                    rectHeight, rounding.x, rounding.x);

            D2D::context->PopAxisAlignedClip();

            FlarialGUI::PushSize(notif.currentPos, notif.currentPosY, rectWidth, rectHeight);

            float logoX = Constraints::PercentageConstraint(0.01, "left") - Constraints::SpacingConstraint(0.18, rectHeight);
            float logoY = Constraints::PercentageConstraint(0.01, "top") - Constraints::SpacingConstraint(0.10, rectHeight);
            float logoWidth = Constraints::RelativeConstraint(1.25);

            FlarialGUI::Image("\\Flarial\\assets\\logo.png", D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            logoX += Constraints::SpacingConstraint(0.85, logoWidth);

            logoY -= Constraints::SpacingConstraint(0.105, logoWidth);
            FlarialGUI::FlarialTextWithFont(logoX, logoY, FlarialGUI::to_wide("Notification").c_str(), D2D1::ColorF(D2D1::ColorF::White), rectWidth, logoWidth, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.45, Constraints::RelativeConstraint(0.45, "height", true)), DWRITE_FONT_WEIGHT_BOLD);

            logoY += Constraints::SpacingConstraint(0.185, logoWidth);
            FlarialGUI::FlarialTextWithFont(logoX, logoY, FlarialGUI::to_wide(notif.text).c_str(), D2D1::ColorF(D2D1::ColorF::White), rectWidth, logoWidth, DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(0.3, Constraints::RelativeConstraint(0.45, "height", true)));

            FlarialGUI::PopSize();

            if(notif.currentPos > Constraints::PercentageConstraint(0.01, "right", true)) notif.finished = true;

        }

        y -= Constraints::SpacingConstraint(1.25, rectHeight);
        if(notif.finished) FlarialGUI::notifications.erase(std::next(FlarialGUI::notifications.begin(), i));
    }
}

void FlarialGUI::BlurRect(D2D1_ROUNDED_RECT rect, float intensity) {


    if(SwapchainHook::init && FlarialGUI::blurbrush != nullptr) {

        if(factory == nullptr) D2D::context->GetFactory(&factory);

        ID2D1RoundedRectangleGeometry* geo;
        factory->CreateRoundedRectangleGeometry(rect, &geo);

        D2D::context->FillGeometry(geo, FlarialGUI::blurbrush);

        Memory::SafeRelease(factory);
        Memory::SafeRelease(geo);
    }

}

void FlarialGUI::AllahBlur(float intensity) {

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
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(FlarialGUI::blur);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(FlarialGUI::blur);
    }

}

void FlarialGUI::ShadowRect(D2D1_ROUNDED_RECT rect) {

    
    // Create a new blank bitmap
    ID2D1Bitmap1* newLayer = nullptr;
    D2D1_BITMAP_PROPERTIES1 newLayerProps = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D::context->GetPixelFormat());
    D2D::context->CreateBitmap(D2D::context->GetPixelSize(), nullptr, 0, newLayerProps, &newLayer);

    if(newLayer != nullptr && FlarialGUI::blur != nullptr) {
        D2D::context->SetTarget(newLayer);
        D2D::context->Clear(D2D1::ColorF(0, 0, 0, 0));

        ID2D1SolidColorBrush *colorBrush = nullptr;
        colorBrush = FlarialGUI::getBrush(D2D1::ColorF(0, 0, 0, 0.75f));
        D2D::context->FillRectangle(rect.rect, colorBrush);

        

        FlarialGUI::blur->SetInput(0, newLayer);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 10.0f);

        ID2D1Image *out;
        FlarialGUI::blur->GetOutput(&out);

        // Set the rendering target to the main bitmap
        if (SwapchainHook::queue != nullptr)
            D2D::context->SetTarget(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap]);
        else D2D::context->SetTarget(SwapchainHook::D2D1Bitmap);


        D2D::context->DrawImage(out);

        Memory::SafeRelease(newLayer);
        Memory::SafeRelease(out);
    }

    Memory::SafeRelease(newLayer);
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
void FlarialGUI::lerp(T& a, const T& b, float t)
{
    // Perform linear interpolation between a and b based on t
    float interpolatedValue = a + (b - a) * t;

    // Round up the interpolated value to three decimal places
    float roundedValue = std::ceilf(interpolatedValue * 1000.0f) / 1000.0f;

    // Assign the rounded value back to 'a'
    a = roundedValue;
}

ID2D1SolidColorBrush* FlarialGUI::getBrush(D2D1_COLOR_F color) {
    std::string key =
            std::to_string(color.r) + std::to_string(color.g) + std::to_string(color.b) + std::to_string(color.a);


    auto it = brushCache.find(key);
    if (it != brushCache.end()) {

        return it->second;
    } else {

        ID2D1SolidColorBrush *brush;

        D2D::context->CreateSolidColorBrush(color, &brush);

        brushCache[key] = brush;

        return brush;
    }
}