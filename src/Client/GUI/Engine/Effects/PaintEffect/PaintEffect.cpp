#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

void FlarialGUI::ApplyPaintEffect(float intensity) {

    if (SwapchainHook::init) {

        ID2D1Effect *morphologyEffect = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1Morphology, &morphologyEffect);

        ID2D1Bitmap *bitmap = nullptr;

        if (SwapchainHook::isDX12)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), &bitmap);

        morphologyEffect->SetInput(0, bitmap);

        // Set blur intensity
        morphologyEffect->SetValue(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_ERODE);
        morphologyEffect->SetValue(D2D1_MORPHOLOGY_PROP_WIDTH, intensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(morphologyEffect);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(morphologyEffect);
    }
}