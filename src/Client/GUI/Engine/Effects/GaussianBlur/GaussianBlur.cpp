#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::ApplyGaussianBlur(float blurIntensity) {
    FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y), 0, 0), 0.0);
}