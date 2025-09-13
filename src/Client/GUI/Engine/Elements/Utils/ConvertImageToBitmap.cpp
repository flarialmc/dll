#include "../../../../Module/Modules/ClickGUI/Elements/ClickGUIElements.hpp"
#include "../../../D2D.hpp"
#include <d2d1helper.h>

void ClickGUIElements::convertImageToBitmap(ID2D1Image *pImg, D2D1_SIZE_U size) {
    ID2D1Image *oldTarget = nullptr;
    ID2D1Bitmap1 *targetBitmap = nullptr;

    //Create a Bitmap with "D2D1_BITMAP_OPTIONS_TARGET"
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_TARGET,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
    D2D::context->CreateBitmap(size, nullptr, 0, bitmapProperties, &targetBitmap);



    //Save current Target, replace by ID2D1Bitmap
    D2D::context->GetTarget(&oldTarget);
    D2D::context->SetTarget(targetBitmap);
    //Draw image on Target (if currently not drawing also call Begin/EndDraw)
    D2D::context->DrawImage(pImg);

    //Set previous Target
    D2D::context->SetTarget(oldTarget);
}