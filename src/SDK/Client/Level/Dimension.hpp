#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "Weather.hpp"

class mcDimension { // is named mcDimension since it conflicts with the Dimension class in Engine.hpp
public:
	BUILD_ACCESS(this, Weather*, weather, 0x1B0);
};