#include "../../../../Engine.hpp"
#include "../../../../../../Client.hpp"

void FlarialGUI::ShadowRect(D2D1_ROUNDED_RECT rect, D2D1_COLOR_F color) {

    if (!Client::settings.getSettingByName<bool>("noshadows")->value) {
        // Create a new blank bitmap
        ID2D1Bitmap1 *newLayer = nullptr;
        D2D1_BITMAP_PROPERTIES1 newLayerProps = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
                                                                        D2D::context->GetPixelFormat());
        D2D::context->CreateBitmap(D2D::context->GetPixelSize(), nullptr, 0, newLayerProps, &newLayer);

        if(FlarialGUI::shadow_blur == nullptr){
            D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::shadow_blur);
            FlarialGUI::shadow_blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
            FlarialGUI::shadow_blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 10.0f);
        }

        if (newLayer != nullptr) {
            D2D::context->SetTarget(newLayer);
            D2D::context->Clear(D2D1::ColorF(0, 0, 0, 0));

            ID2D1SolidColorBrush *colorBrush = nullptr;
            colorBrush = FlarialGUI::getBrush(color);
            D2D::context->FillRoundedRectangle(rect, colorBrush);


            FlarialGUI::shadow_blur->SetInput(0, newLayer);

            ID2D1Image *out;
            FlarialGUI::shadow_blur->GetOutput(&out);

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
}