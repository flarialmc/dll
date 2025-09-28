#pragma once

class BlockSource;
class BlockLegacy;

class Block {
public:
    BlockLegacy* getBlockLegacy() const;
    mce::Color getMapColor(BlockSource& region, const BlockPos& pos) const;
};