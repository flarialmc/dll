#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplyHue(float Hue) {

    if (SwapchainHook::init) {

        winrt::com_ptr<ID2D1Effect> cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1Saturation, cum.put());

        winrt::com_ptr<ID2D1Bitmap> bitmap = nullptr;

        if (SwapchainHook::queue != nullptr)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), bitmap.put());
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), bitmap.put());

        cum->SetInput(0, bitmap.get());

        // Set blur intensity
        cum->SetValue(D2D1_SATURATION_PROP_SATURATION, Hue);
        D2D::context->DrawImage(cum.get());
    }
}