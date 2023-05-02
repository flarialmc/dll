#pragma once
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"

class Level {
public:
	auto getRuntimeEntity(uint64_t, bool) -> class Actor*;
};