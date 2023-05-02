
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

  static void RoundedButton(const float x, const float y, const D2D_COLOR_F color, const float width = 100.0f, const float height = 160.0f, const float radiusX = 10.0f, const float radiusY = 10.0f)
    {

    ID2D1SolidColorBrush* brush;
    RenderUtils::D2DC->CreateSolidColorBrush(color, &brush);
    
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    RenderUtils::D2DC->FillRoundedRectangle(roundedRect, brush);
    brush->Release();
    
    }
};
