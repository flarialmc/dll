#include "../../Engine.hpp"

void FlarialGUI::ApplyGaussianBlur() {
    FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y), 0, 0));
}