#pragma once

#include <cstdint>
#include <Utils/Utils.hpp>
#include <Utils/VersionUtils.hpp>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <SDK/Client/Actor/EntityContext.hpp>

#include "SDK/Client/Core/HashedString.hpp"


// Minimal AttributeInstance — we only need getValue().
// The game's AttributeInstance is ~128 bytes with vtable, vectors, shared_ptr, etc.
// We just use hat::member_at to read the value float at the correct offset.
class AttributeInstance {
public:
    float GetValue() const {
        return hat::member_at<float>(const_cast<AttributeInstance*>(this), GET_OFFSET("AttributeInstance::Value"));
    }

    float getValue() const { return GetValue(); }
};

// Minimal Attribute type for legacy compatibility
class Attribute {
public:
    int8_t mRedefinitionNode{0};
    bool mSyncable{true};
    short unk{0};
    int mIDValue{0};
    HashedString mName;

    Attribute(int id = 0, std::string const& name = "") : mIDValue(id), mName(name.c_str()) {}
};

// Raw std::vector layout on MSVC x64 (3 pointers = 24 bytes)
struct RawVec {
    uintptr_t first;
    uintptr_t last;
    uintptr_t end_cap;

    [[nodiscard]] size_t byteSize() const { return last - first; }
};

struct AttributesComponent : IEntityComponent {
    // Game's BaseAttributeMap layout (88 bytes in both old and new versions):
    //
    // Pre-1.21.13X (unordered_map based):
    //   [0x00] std::unordered_map<uint, AttributeInstance> mInstanceMap (64 bytes)
    //   [0x40] std::vector<AttributeInstanceHandle> mDirtyAttributes (24 bytes)
    //
    // 1.21.13X+ (brstd::flat_map based):
    //   [0x00] brstd::flat_map<uint, AttributeInstance, ...> mInstanceMap (56 bytes)
    //     [0x00] std::vector<uint> keys (24 bytes)
    //     [0x18] std::vector<AttributeInstance> values (24 bytes)
    //     [0x30] comparator + padding (8 bytes)
    //   [0x38] std::vector<AttributeInstanceHandle> mDirtyAttributes (24 bytes)
    //   [0x50] void (BaseAttributeMap::*)(const AttributeInstance&) mOnAttributeModified (8 bytes)
    alignas(8) std::byte storage[88];

    AttributeInstance* getInstance(unsigned int id) {
        if (VersionUtils::checkAboveOrEqual(21, 130)) {
            return getInstanceFlat(id);
        }
        return getInstanceLegacy(id);
    }

private:
    // 1.21.13X+: brstd::flat_map stores sorted key/value vectors.
    // We iterate the keys vector to find our ID, then index into the values vector.
    AttributeInstance* getInstanceFlat(unsigned int id) {
        auto& keys = *reinterpret_cast<RawVec*>(&storage[0]);
        auto& vals = *reinterpret_cast<RawVec*>(&storage[24]);

        if (!keys.first || !vals.first) return nullptr;

        size_t n = keys.byteSize() / sizeof(unsigned int);
        if (n == 0 || n > 200) return nullptr;

        size_t valBytes = vals.byteSize();
        if (valBytes == 0) return nullptr;

        // Compute stride from the actual game data instead of hardcoding sizeof(AttributeInstance)
        size_t stride = valBytes / n;
        if (stride == 0 || stride > 512) return nullptr;

        auto* kp = reinterpret_cast<unsigned int*>(keys.first);
        for (size_t i = 0; i < n; i++) {
            if (kp[i] == id) {
                return reinterpret_cast<AttributeInstance*>(vals.first + i * stride);
            }
        }
        return nullptr;
    }

    // Pre-1.21.13X: call game's getMutableInstance via signature-resolved function pointer
    AttributeInstance* getInstanceLegacy(unsigned int id) {
        using func_t = AttributeInstance* (*)(void*, unsigned int);
        auto addr = GET_SIG_ADDRESS("BaseAttributeMap_getInstance");
        auto func = reinterpret_cast<func_t>(addr);
        return func ? func(storage, id) : nullptr;
    }
};

static_assert(sizeof(AttributesComponent) == 88);
