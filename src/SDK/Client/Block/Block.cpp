#include "Block.hpp"

#include "BlockLegacy.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

BlockLegacy *Block::getBlockLegacy() const {
    return hat::member_at<BlockLegacy*>(this, GET_OFFSET("Block::blockLegacy"));
}

mce::Color Block::getMapColor(BlockSource& region, const BlockPos& pos) const
{
    return this->getBlockLegacy()->getMapColor(region, pos, *this);
}

