#include "../../../Engine.hpp"

void FlarialGUI::Circle(float x, float y, const D2D1_COLOR_F &color, float radius) {

    // Create a brush using the specified color
    //ID2D1SolidColorBrush *brush;

    //brush = FlarialGUI::getBrush(color).get();

    // Create an ellipse with the specified parameters

    /*
    D2D1_ELLIPSE ellipse;
    ellipse.point = D2D1::Point2F(x, y);
    ellipse.radiusX = radius;
    ellipse.radiusY = radius;
    */

    //ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, D2DColorToImColor(color));
    ImGui::GetBackgroundDrawList()->AddEllipseFilled(ImVec2(x, y), ImVec2(radius, radius), D2DColorToImColor(color), 0, 0);

    // Draw the ellipse using the device context and brush
    // D2D::context->FillEllipse(ellipse, brush);
}