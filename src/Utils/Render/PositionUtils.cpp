#include "PositionUtils.hpp"
#include "../../SDK/SDK.hpp"

Vec2<float> PositionUtils::getUIScale() {
    auto screenSize = SDK::clientInstance->getGuiData()->getScreenSize();
    auto uiSize = SDK::clientInstance->getGuiData()->getScreenSizeScaled();
    return uiSize.div(screenSize);
}

Vec2<float> PositionUtils::getScaledPos(Vec2<float> pos) {
    Vec2<float> uiScale = getUIScale();
    return pos.mul(uiScale);
}

Vec2<float> PositionUtils::getScreenScaledPos(Vec2<float> pos) {
    Vec2<float> uiScale = getUIScale();
    return pos.div(uiScale);
}

Vec2<float> PositionUtils::getCustomScreenScaledPos(Vec2<float> pos, float scale) {
    return getScreenScaledPos(pos).mul(scale);
}
