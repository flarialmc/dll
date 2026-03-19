#pragma once

#include <Utils/Utils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <libhat.hpp>

class BlockLegacy;

class Item {
public:
    uintptr_t** vtable; // 0x0

    BUILD_ACCESS(this, std::string, AtlasTextureFile, GET_OFFSET("Item::AtlasTextureFile"));
    BUILD_ACCESS(this, std::string, Namespace, GET_OFFSET("Item::Namespace"));
    BUILD_ACCESS(this, std::string, name, GET_OFFSET("Item::name"));

    // Returns the BlockLegacy (block type) associated with this item, if any.
    // Reads the mBlockType WeakPtr<BlockLegacy> field at the version-specific offset.
    // Returns nullptr for non-block items (tools, food, etc.).
    BlockLegacy* getBlockLegacy() {
        // WeakPtr<BlockLegacy> is just a SharedCounter<BlockLegacy>* (8 bytes).
        // SharedCounter<T> stores T* ptr at its first field (offset 0).
        auto counter = hat::member_at<uintptr_t>(this, GET_OFFSET("Item::mBlockType"));
        if (!counter) return nullptr;
        return *reinterpret_cast<BlockLegacy**>(counter);
    }
};