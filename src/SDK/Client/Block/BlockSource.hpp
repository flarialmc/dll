#pragma once

#include "Block.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class BlockSource {
public:
	Block* getBlock(const Vec3<int>&);
};