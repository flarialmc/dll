#pragma once

#include "../../../Utils/Memory/Memory.hpp"

class Weather {
public:
	BUILD_ACCESS(this, float, lightingLevel, 0x44);
	BUILD_ACCESS(this, float, rainLevel, 0x38);
	
};