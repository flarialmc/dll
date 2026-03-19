#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

void FlarialGUI::ApplyCombinedDeepFry() {

    if (SwapchainHook::init) {

        ID2D1Effect *discreteTransferEffect = nullptr;
        ID2D1Effect *edgeDetectionEffect = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, &discreteTransferEffect);
        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, &edgeDetectionEffect);

        ID2D1Bitmap *bitmap = nullptr;

        if (SwapchainHook::isDX12)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), &bitmap);

        discreteTransferEffect->SetInput(0, bitmap);

        // Set blur intensity
        float table[3] = {0.0f, 0.5f, 1.0f};
        discreteTransferEffect->SetValue(D2D1_DISCRETETRANSFER_PROP_RED_TABLE, table);
        discreteTransferEffect->SetValue(D2D1_DISCRETETRANSFER_PROP_GREEN_TABLE, table);
        discreteTransferEffect->SetValue(D2D1_DISCRETETRANSFER_PROP_BLUE_TABLE, table);

        ID2D1Image *bitmap2;
        discreteTransferEffect->GetOutput(&bitmap2);
        edgeDetectionEffect->SetInput(0, bitmap2);

        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(edgeDetectionEffect);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(bitmap2);
        Memory::SafeRelease(discreteTransferEffect);
        Memory::SafeRelease(edgeDetectionEffect);
    }
}