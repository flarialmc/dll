#include "../../../Engine.hpp"

void FlarialGUI::ColorWheel(float x, float y, float radius) {
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
    ID2D1GradientStopCollection *gradientStopCollection = nullptr;
    D2D::context->CreateGradientStopCollection(
            gradientStops,
            7,
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            &gradientStopCollection
    );

    // Create the radial gradient brush with appropriate properties
    ID2D1RadialGradientBrush *radialBrush = nullptr;
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