#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../../Assets/Assets.hpp"


void FlarialGUI::image(const std::string& imageName, D2D1_RECT_F rect) {

    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    if (shouldAdditionalY) {
        rect.top += additionalY[additionalIndex];
        rect.bottom += additionalY[additionalIndex];
    }

    std::string among = Utils::getRoamingPath() + "\\" + imageName;

    if (ImagesClass::eimages[imageName] == nullptr)
        LoadImageFromFile(FlarialGUI::to_wide(among).c_str(), ImagesClass::eimages[imageName].put());

    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

    if (imageName == R"(\Flarial\assets\transparent.png)")
        interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

    if (isInScrollView) {
        if (isRectInRect(ScrollViewRect, rect))
            D2D::context->DrawBitmap(ImagesClass::eimages[imageName].get(), imageRect, 1.0f,
                                     interpolationMode);
    } else {
        D2D::context->DrawBitmap(ImagesClass::eimages[imageName].get(), imageRect, 1.0f,
                                 interpolationMode);
    }
}

void FlarialGUI::image(int resourceId, D2D1_RECT_F rect, LPCTSTR type) {

    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    if (shouldAdditionalY) {
        rect.top += additionalY[additionalIndex];
        rect.bottom += additionalY[additionalIndex];
    }

    if (ImagesClass::images[resourceId] == nullptr)
        LoadImageFromResource(resourceId, ImagesClass::images[resourceId].put(), type);


    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

    if (resourceId == IDR_TRANSPARENT_PNG)
        interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

    if (isInScrollView) {
        if (isRectInRect(ScrollViewRect, rect))
            D2D::context->DrawBitmap(ImagesClass::images[resourceId].get(), imageRect, 1.0f,
                                     interpolationMode);
    } else {
        D2D::context->DrawBitmap(ImagesClass::images[resourceId].get(), imageRect, 1.0f,
                                 interpolationMode);
    }
}