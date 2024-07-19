#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplySusGaussianBlur(float blurIntensity) {

    ID2D1Effect *effect;
    D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &effect);

    if (SwapchainHook::init) {

        ID2D1Image *input;
        D2D::context->GetTarget(&input);

        effect->SetInput(0, input);

        // Set blur intensity
        effect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurIntensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(effect);

        Memory::SafeRelease(input);
        Memory::SafeRelease(effect);
    }
}