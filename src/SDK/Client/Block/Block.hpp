#pragma once

#include "SDK/SDK.hpp"

class BlockLegacy;

class Block {
public:
    BlockLegacy* getBlockLegacy();

    static bool isAir(BlockPos const & pos);
};