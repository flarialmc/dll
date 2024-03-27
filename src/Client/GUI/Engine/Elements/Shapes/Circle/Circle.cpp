#include "../../../Engine.hpp"

void FlarialGUI::Circle(float x, float y, const D2D1_COLOR_F &color, float radius) {

    // Create a brush using the specified color
    ID2D1SolidColorBrush *brush;

    brush = FlarialGUI::getBrush(color);

    // Create an ellipse with the specified parameters
    D2D1_ELLIPSE ellipse;
    ellipse.point = D2D1::Point2F(x, y);
    ellipse.radiusX = radius;
    ellipse.radiusY = radius;

    // Draw the ellipse using the device context and brush
    D2D::context->FillEllipse(ellipse, brush);
}