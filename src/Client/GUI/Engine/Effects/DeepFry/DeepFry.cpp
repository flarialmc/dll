#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

void FlarialGUI::ApplyDeepFry(float intensity) {

    if (SwapchainHook::init) {

        ID2D1Effect *edgeDetectionEffect = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, &edgeDetectionEffect);

        ID2D1Bitmap *bitmap = nullptr;

        if (SwapchainHook::isDX12)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), &bitmap);

        edgeDetectionEffect->SetInput(0, bitmap);

        // Set blur intensity
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, intensity);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        edgeDetectionEffect->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(edgeDetectionEffect);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(edgeDetectionEffect);
    }
}