#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <SDK/Client/Core/HashedString.hpp>
#include "ItemRegistryRef.hpp"
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

// Utility for validating item names against the game's ItemRegistry.
//
// The registry's mNameToItemMap is an unordered_map<HashedString, WeakPtr<Item>>
// where WeakPtr<Item> is the game's custom 8-byte pointer wrapper.
// We reinterpret it as unordered_map<HashedString, uintptr_t> (same value size)
// and use count() for O(1) lookups. This works because:
//   - Same key type (HashedString) with matching hash and equality
//   - Same value size (8 bytes)
//   - Same compiler (MSVC) = same STL node layout
namespace ItemRegistryHelper {

    // Type alias for the reinterpreted map (value type matches WeakPtr<Item> size).
    using ItemNameMap = std::unordered_map<HashedString, uintptr_t>;

    // Check if a fully-qualified item name (e.g. "minecraft:diamond") exists in the registry.
    // Returns false if the registry is unavailable or the item is not found.
    inline bool isItemInRegistry(const ItemRegistryRef& registryRef, const std::string& fullItemName) {
        auto registry = registryRef.lockRegistry();
        if (!registry) return false;

        static int mapOffset = GET_OFFSET("ItemRegistry::mNameToItemMap");
        auto* nameMap = reinterpret_cast<const ItemNameMap*>(
            reinterpret_cast<const char*>(registry.get()) + mapOffset);

        HashedString key(fullItemName);
        return nameMap->count(key) > 0;
    }

    // Check if a short item name (without namespace, e.g. "diamond") exists
    // in the tile item name map. This handles block items that register
    // under their short name as well.
    inline bool isItemInTileNameMap(const ItemRegistryRef& registryRef, const std::string& shortName) {
        auto registry = registryRef.lockRegistry();
        if (!registry) return false;

        static int mapOffset = GET_OFFSET("ItemRegistry::mTileItemNameToItemMap");
        auto* tileNameMap = reinterpret_cast<const ItemNameMap*>(
            reinterpret_cast<const char*>(registry.get()) + mapOffset);

        HashedString key(shortName);
        return tileNameMap->count(key) > 0;
    }

    // High-level validation: checks the item name against the registry.
    // Tries the full name first, then common prefixed variants.
    // Returns true if the item exists under any recognized form.
    inline bool validateItemName(const ItemRegistryRef& registryRef,
                                 const std::string& normalizedInput,
                                 const std::string& namespacePart,
                                 const std::string& itemPart,
                                 bool hasNamespace) {
        if (!registryRef.valid()) return false;

        // If user specified a namespace, try the exact full name
        if (hasNamespace) {
            if (isItemInRegistry(registryRef, normalizedInput)) return true;
            // Also try the tile name map with just the item part
            if (isItemInTileNameMap(registryRef, itemPart)) return true;
            return false;
        }

        // No namespace specified — try common prefixes
        if (isItemInRegistry(registryRef, "minecraft:" + itemPart)) return true;
        if (isItemInRegistry(registryRef, itemPart)) return true;
        if (isItemInTileNameMap(registryRef, itemPart)) return true;

        return false;
    }
}
