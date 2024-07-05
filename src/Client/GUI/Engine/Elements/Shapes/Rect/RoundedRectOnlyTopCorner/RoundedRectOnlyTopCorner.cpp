#include "../../../../Engine.hpp"
#include "../../../../../../../Utils/Memory/Memory.hpp"

void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width, float height, float radiusX,
                                          float radiusY) {
    if (isInScrollView) y += scrollpos;
    if (shouldAdditionalY) {
        for (int i = 0; i < highestAddIndexes + 1; i++) {
            if (FlarialGUI::DropDownMenus[i].isActive && i <= additionalIndex) {
                y += additionalY[i];
            }
        }
    }

    const bool isAdditionalY = shouldAdditionalY;

    if (isAdditionalY) UnSetIsInAdditionalYMode();

    if (isInScrollView && !isRectInRect(ScrollViewRect, D2D1::RectF(x, y, x + width, y + height))) return;

    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    if (factory == nullptr)
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

    brush = FlarialGUI::getBrush(color).get();

    D2D::context->FillGeometry(geometry, brush);

    Memory::SafeRelease(sink);
    Memory::SafeRelease(geometry);
    Memory::SafeRelease(factory);

    if (isAdditionalY) SetIsInAdditionalYMode();
}