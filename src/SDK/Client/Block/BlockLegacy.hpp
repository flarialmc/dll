#pragma once

class BlockLegacy {
public:
    std::string getName();
    std::string getNamespace();

    mce::Color getMapColor(BlockSource& region, const BlockPos& pos, const Block& block)
    {
        static int off = GET_OFFSET("BlockLegacy::getMapColor");
        return Memory::CallVFuncI<mce::Color, BlockSource&, const BlockPos&, const Block&>(off, this, region, pos, block);

    }

};
