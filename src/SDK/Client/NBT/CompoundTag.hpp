#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <Utils/Memory/Memory.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

// Forward declarations
class ListTag;
class CompoundTag;

// ============================================================================
// Tag Type IDs (from IDA analysis of 1.21.13X client)
// ============================================================================
namespace TagType {
    constexpr uint8_t End = 0;
    constexpr uint8_t Byte = 1;
    constexpr uint8_t Short = 2;
    constexpr uint8_t Int = 3;
    constexpr uint8_t Int64 = 4;
    constexpr uint8_t Float = 5;
    constexpr uint8_t Double = 6;
    constexpr uint8_t ByteArray = 7;
    constexpr uint8_t String = 8;
    constexpr uint8_t List = 9;
    constexpr uint8_t Compound = 10;
    constexpr uint8_t IntArray = 11;
}

// ============================================================================
// CompoundTagVariant - Internal storage for tag values in CompoundTag's map
// ============================================================================
// Based on IDA analysis of 1.21.13X client:
// - Located within std::map tree nodes at node+64 (node+8 is the variant)
// - Offset +0: 8 bytes padding/alignment
// - Offset +8: value storage (32 bytes, varies by type)
// - Offset +40 (0x28): tag type ID (uint8_t)
//
// The type check in getByte/getShort/etc. is: cmp byte ptr [rax+28h], TYPE
// Where rax = variant pointer (rbp + 40h = node + 8 + 40h = node + 48h)
struct CompoundTagVariant {
    char pad[8];          // +0: alignment
    char valueData[32];   // +8: value storage (varies by type)
    uint8_t typeId;       // +40 (0x28): tag type ID

    [[nodiscard]] uint8_t getType() const { return typeId; }

    template<typename T>
    [[nodiscard]] T getValue() const {
        return *reinterpret_cast<const T*>(valueData);
    }

    // For List type, the list pointers are at valueData
    // valueData+0: begin pointer
    // valueData+8: end pointer
    [[nodiscard]] void* const* getListBegin() const {
        return *reinterpret_cast<void* const* const*>(valueData);
    }

    [[nodiscard]] void* const* getListEnd() const {
        return *reinterpret_cast<void* const* const*>(valueData + 8);
    }
};

// ============================================================================
// ListTag - NBT list tag containing ordered elements
// ============================================================================
// Based on IDA analysis:
// - When embedded in CompoundTagVariant, begin ptr at variant+8, end ptr at variant+16
// - For standalone ListTag: +0 vtable, +8 begin, +16 end
// - size() = (end - begin) >> 3
class ListTag {
public:
    uintptr_t vtable;      // +0: vtable pointer
    void** mBegin;         // +8: vector begin pointer
    void** mEnd;           // +16: vector end pointer

    // Get the number of elements in the list
    // From IDA: mov rax, [rcx+10h]; sub rax, [rcx+8]; sar rax, 3; ret
    [[nodiscard]] int size() const {
        const auto begin = reinterpret_cast<uintptr_t>(mBegin);
        const auto end = reinterpret_cast<uintptr_t>(mEnd);
        return static_cast<int>((end - begin) >> 3);
    }

    // Get element at index (returns raw pointer to unique_ptr contents)
    [[nodiscard]] void* get(size_t index) const {
        if (static_cast<int>(index) >= size()) return nullptr;
        return mBegin[index];
    }

    // Get element as CompoundTag at index
    // Items in list are stored as std::unique_ptr<Tag>
    [[nodiscard]] const CompoundTag* getCompound(size_t index) const;
};

// ============================================================================
// CompoundTag - NBT compound tag containing named key-value pairs
// ============================================================================
// Based on IDA analysis of 1.21.13X client:
// - Uses std::map<std::string, CompoundTagVariant> at offset +8
// - get() function searches map by string_view (16-byte struct in xmm register)
// - Getter functions (getByte, getShort, etc.) call get() then check type
//
// The get() function signature:
//   CompoundTagVariant* __fastcall get(CompoundTag* this, __m128i* string_view)
// Where string_view is {const char* ptr, size_t len}
class CompoundTag {
private:
    uintptr_t vtable = 0;  // +0: vtable pointer

    // Internal struct to pass string_view to game functions
    struct StringView {
        const char* ptr;
        size_t len;
    };

    // Helper to find a tag variant by key using game's get function
    // Returns pointer to CompoundTagVariant or nullptr
    [[nodiscard]] const CompoundTagVariant* findVariant(std::string_view key) const {
        // The game's get function takes this and a string_view passed via XMM register
        // It returns the variant pointer if found and type is valid (0-11), else nullptr
        static auto getFunc = reinterpret_cast<const CompoundTagVariant*(__fastcall*)(
            const CompoundTag*, const StringView*)>(
            GET_SIG_ADDRESS("CompoundTag::get"));

        if (getFunc) {
            const StringView sv{ key.data(), key.size() };
            return getFunc(this, &sv);
        }
        return nullptr;
    }

public:
    // Check if a key exists in the compound tag
    [[nodiscard]] bool contains(std::string_view key) const {
        return findVariant(key) != nullptr;
    }

    // Get a ListTag by key (type == 9)
    // Returns pointer to the ListTag within the variant, or nullptr
    [[nodiscard]] const ListTag* getList(std::string_view key) const {
        if (auto* variant = findVariant(key); variant && variant->getType() == TagType::List) {
            // For List types, the variant IS the ListTag
            // (vtable at variant+0, begin/end pointers at variant+8/+16)
            return reinterpret_cast<const ListTag*>(variant);
        }
        return nullptr;
    }

    // Get a nested CompoundTag by key (type == 10)
    // Note: We use findVariant + type check instead of a direct signature because
    // all getter functions (getInt, getString, getCompound, etc.) share identical
    // prologue bytes and only differ at the type check value at byte 263.
    [[nodiscard]] const CompoundTag* getCompound(std::string_view key) const {
        auto* variant = findVariant(key);
        if (variant && variant->getType() == TagType::Compound) {
            // For Compound types, the variant IS the CompoundTag
            // (vtable at variant+0, map data at variant+8)
            return reinterpret_cast<const CompoundTag*>(variant);
        }
        return nullptr;
    }

    // Get a byte value (type == 1)
    // Note: Using fallback implementation because list element CompoundTags
    // have different internal structure than the game's getByte expects
    [[nodiscard]] uint8_t getByte(std::string_view key) const {
        if (auto* variant = findVariant(key); variant && variant->getType() == TagType::Byte) {
            return variant->getValue<uint8_t>();
        }
        return 0;
    }

    // Get a short value (type == 2)
    [[nodiscard]] int16_t getShort(std::string_view key) const {
        if (auto* variant = findVariant(key); variant && variant->getType() == TagType::Short) {
            return variant->getValue<int16_t>();
        }
        return 0;
    }

    // Get an int value (type == 3)
    [[nodiscard]] int32_t getInt(std::string_view key) const {
        if (auto* variant = findVariant(key); variant && variant->getType() == TagType::Int) {
            return variant->getValue<int32_t>();
        }
        return 0;
    }

    // Get a string value (type == 8)
    // Returns pointer to the std::string stored in the variant
    [[nodiscard]] const std::string* getString(std::string_view key) const {
        auto* variant = findVariant(key);
        if (variant && variant->getType() == TagType::String) {
            return reinterpret_cast<const std::string*>(variant->valueData);
        }
        return nullptr;
    }

    // Get the number of entries in this compound tag
    // MSVC x64 std::map layout varies by version, so we use a safer approach
    // by accessing the map's _Mysize through the tree structure
    [[nodiscard]] size_t size() const {
        // CompoundTag layout: vtable (+0), map data (+8)
        // MSVC std::map in release mode (x64):
        // +0: _Myhead pointer (tree header node)
        // +8: _Mysize (number of elements)
        auto* mapPtr = reinterpret_cast<const char*>(this) + 8;
        const auto mapSize = *reinterpret_cast<const size_t*>(mapPtr + 8);
        return mapSize;
    }
};

// Implementation of ListTag::getCompound that depends on CompoundTag definition
inline const CompoundTag* ListTag::getCompound(size_t index) const {
    const void* element = get(index);
    if (!element) return nullptr;

    // mBegin[index] gives us the Tag* directly (unique_ptr is just a raw pointer internally)
    return static_cast<const CompoundTag*>(element);
}
