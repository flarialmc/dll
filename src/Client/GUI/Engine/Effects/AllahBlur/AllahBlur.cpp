#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::AllahBlur(float intensity) {

    /*
    // Create Gaussian blur effect
    if (FlarialGUI::blur == nullptr) {

        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);

        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);
    }

    if (SwapchainHook::init) {
        if (SwapchainHook::queue != nullptr)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap],
                                   &FlarialGUI::screen_bitmap_cache);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &FlarialGUI::screen_bitmap_cache);

        FlarialGUI::blur->SetInput(0, FlarialGUI::screen_bitmap_cache);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
        FlarialGUI::blur->GetOutput(&FlarialGUI::blur_bitmap_cache);

        // Draw the image with the Gaussian blur effect
        if (FlarialGUI::blur_bitmap_cache != nullptr)
            D2D::context->DrawImage(FlarialGUI::blur_bitmap_cache);
    }*/

}