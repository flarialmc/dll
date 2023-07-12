#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Utils.hpp"

class GuiData {
	BUILD_ACCESS(this, Vec2<float>, ScreenSize, 0x0030);
	BUILD_ACCESS(this, Vec2<float>, ScreenSizeScaled, 0x0040);
	BUILD_ACCESS(this, float, GuiScale, 0x004C);
};