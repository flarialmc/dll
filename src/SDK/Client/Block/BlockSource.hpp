#pragma once

#include "../Level/Dimension.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../Level/Biome.hpp"
#include "../Level/Level.hpp"
#include "Block.hpp"

class BlockSource {
public:
    Block *getBlock(BlockPos const & pos);

    Dimension* getDimension();

    Biome *getBiome(BlockPos const & bp);
};