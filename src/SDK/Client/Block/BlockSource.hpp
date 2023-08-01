#pragma once

#include "Block.hpp"
#include "../Level/Dimension.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class BlockSource {
public:

	BUILD_ACCESS(this, mcDimension*, dimension, 0x30);

	Block* getBlock(const Vec3<int>&);
};