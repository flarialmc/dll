#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplyAestheticDeepFry() {

    if (SwapchainHook::init) {

        ID2D1Effect *cum = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, &cum);

        ID2D1Bitmap *bitmap = nullptr;

        if (SwapchainHook::queue != nullptr)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        cum->SetInput(0, bitmap);

        // Set blur intensity
        float table[3] = {0.0f, 0.5f, 1.0f};
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_RED_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_GREEN_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_BLUE_TABLE, table);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(cum);
    }
}