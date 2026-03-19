#pragma once

#include "SDK/Client/Render/Textures/Color.hpp"
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <libhat.hpp>

class Block;

class BlockLegacy {
public:
    std::string getName();
    std::string getNamespace();

    mce::Color getMapColor() {
        return hat::member_at<mce::Color>(this, GET_OFFSET("BlockLegacy::mMapColor"));
    }

    // Returns the default Block permutation (data=0) for this block type.
    // In 1.21.13x+ this class is actually BlockType. The field is BlockType::mDefaultState at +0x2E0.
    // WARNING: +0x2C0 is BlockType::mCreativeEnumState (uint64) — reads 0xFFFFFFFF for fence gates.
    // Correct offset 0x2E0 confirmed via ItemStack_getLinkedBlockPtr IDA analysis.
    Block* getDefaultState() {
        return hat::member_at<Block*>(this, GET_OFFSET("BlockLegacy::mDefaultState"));
    }
};
