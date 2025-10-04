#pragma once

#include "../../../Utils/Utils.hpp"
#include "../Level/HitResult.hpp"

// Forward declarations
class Block;
class Dimension;
class Biome;
class Level;

class BlockSource {
public:
    Block *getBlock(BlockPos const & pos);

    Dimension* getDimension();

    Biome *getBiome(BlockPos const & bp);
};