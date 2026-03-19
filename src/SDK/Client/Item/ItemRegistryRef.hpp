#pragma once

#include <memory>

// Minimal wrapper for the game's ItemRegistryRef class.
// The game's ItemRegistryRef holds a std::weak_ptr<ItemRegistry> (NOT the custom WeakPtr).
// ItemRegistry itself inherits std::enable_shared_from_this<ItemRegistry>.
//
// Binary layout: exactly 16 bytes (std::weak_ptr = stored_ptr + control_block).
struct ItemRegistryRef {
    std::weak_ptr<void> mWeakRegistry;

    // Lock the weak reference to get a shared_ptr keeping the registry alive.
    // Returns nullptr-equivalent shared_ptr if the registry has been destroyed.
    [[nodiscard]] std::shared_ptr<void> lockRegistry() const {
        return mWeakRegistry.lock();
    }

    [[nodiscard]] bool valid() const {
        return !mWeakRegistry.expired();
    }
};

static_assert(sizeof(ItemRegistryRef) == 16, "ItemRegistryRef must be exactly 16 bytes (std::weak_ptr)");
