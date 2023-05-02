#pragma once
#include "MinecraftUIRenderContext.h"

class Font;

class MinecraftGame {
public:
	BUILD_ACCESS(this, bool, canUseKeys, 0xD8);
	BUILD_ACCESS(this, Font*, mcFont, 0xE68);
};