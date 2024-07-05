#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Utils.hpp"

class GuiData {
public:
    BUILD_ACCESS(this, Vec2<float>, ScreenSize, GET_OFFSET("GuiData::ScreenSize"));
    BUILD_ACCESS(this, Vec2<float>, ScreenSizeScaled, GET_OFFSET("GuiData::ScreenSizeScaled"));
    BUILD_ACCESS(this, float, GuiScale, GET_OFFSET("GuiData::GuiScale"));
    BUILD_ACCESS(this, float, screenResRounded, GET_OFFSET("GuiData::screenResRounded"));
    BUILD_ACCESS(this, float, sliderAmount, GET_OFFSET("GuiData::sliderAmount"));
    BUILD_ACCESS(this, float, scalingMultiplier, GET_OFFSET("GuiData::scalingMultiplier"));
};