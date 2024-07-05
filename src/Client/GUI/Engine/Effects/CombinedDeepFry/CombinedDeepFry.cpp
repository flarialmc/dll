#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplyCombinedDeepFry() {

    if (SwapchainHook::init) {

        ID2D1Effect *cum = nullptr;
        ID2D1Effect *cum2 = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, &cum);
        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, &cum2);

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

        ID2D1Image *bitmap2;
        cum->GetOutput(&bitmap2);
        cum2->SetInput(0, bitmap2);

        cum2->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum2);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(bitmap2);
        Memory::SafeRelease(cum);
        Memory::SafeRelease(cum2);
    }
}