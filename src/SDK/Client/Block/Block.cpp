#include "Block.hpp"

#include "BlockLegacy.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

BlockLegacy *Block::getBlockLegacy() {
    return hat::member_at<BlockLegacy*>(this, GET_OFFSET("Block::blockLegacy"));
}
