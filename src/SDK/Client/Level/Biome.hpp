#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "Weather.hpp"

class Biome { // is named mcDimension since it conflicts with the Dimension class in Engine.hpp
public:
	BUILD_ACCESS(this, float, temparature, 0x40);
};