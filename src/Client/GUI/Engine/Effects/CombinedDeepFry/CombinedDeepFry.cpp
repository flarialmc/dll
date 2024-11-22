#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplyCombinedDeepFry() {

    if (SwapchainHook::init) {

        winrt::com_ptr<ID2D1Effect> cum = nullptr;
        winrt::com_ptr<ID2D1Effect> cum2 = nullptr;

        D2D::context->CreateEffect(CLSID_D2D1DiscreteTransfer, cum.put());
        D2D::context->CreateEffect(CLSID_D2D1EdgeDetection, cum2.put());

        winrt::com_ptr<ID2D1Bitmap> bitmap = nullptr;

        if (SwapchainHook::queue != nullptr)
            FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), bitmap.put());
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), bitmap.put());

        cum->SetInput(0, bitmap.get());

        // Set blur intensity
        float table[3] = {0.0f, 0.5f, 1.0f};
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_RED_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_GREEN_TABLE, table);
        cum->SetValue(D2D1_DISCRETETRANSFER_PROP_BLUE_TABLE, table);

        winrt::com_ptr<ID2D1Image> bitmap2;
        cum->GetOutput(bitmap2.put());
        cum2->SetInput(0, bitmap2.get());

        cum2->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
        cum2->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(cum2.get());
    }
}