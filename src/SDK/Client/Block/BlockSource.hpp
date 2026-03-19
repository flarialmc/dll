#pragma once

#include "../Level/Dimension.hpp"
#include "../../../Utils/Utils.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../Level/Biome.hpp"
#include "../Level/Level.hpp"
#include "Block.hpp"

class LevelChunk;

class BlockSource {
public:
    Block *getBlock(BlockPos const & pos);

    // Returns the LevelChunk at chunk coordinates (x, z), or null if not loaded.
    LevelChunk *getChunk(int x, int z);

    Dimension* getDimension();

    Biome *getBiome(BlockPos const & bp);
};