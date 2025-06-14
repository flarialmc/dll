#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>

// Forward declarations
class CompoundTag;
class ListTag;

// Basic NBT Tag types for ShulkerPreview functionality
class Tag {
public:
    enum class Type : uint8_t {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        ByteArray = 7,
        String = 8,
        List = 9,
        Compound = 10
    };

    virtual ~Tag() = default;
    virtual Type getType() const = 0;
};

class ByteTag : public Tag {
private:
    uint8_t value;
public:
    ByteTag(uint8_t val) : value(val) {}
    Type getType() const override { return Type::Byte; }
    uint8_t getValue() const { return value; }
};

class StringTag : public Tag {
private:
    std::string value;
public:
    StringTag(const std::string& val) : value(val) {}
    Type getType() const override { return Type::String; }
    const std::string& getValue() const { return value; }
};

class CompoundTag : public Tag {
private:
    std::unordered_map<std::string, std::unique_ptr<Tag>> tags;
public:
    Type getType() const override { return Type::Compound; }
    
    bool contains(const std::string& key) const {
        return tags.find(key) != tags.end();
    }
    
    void put(const std::string& key, std::unique_ptr<Tag> tag) {
        tags[key] = std::move(tag);
    }
    
    const Tag* get(const std::string& key) const {
        auto it = tags.find(key);
        return (it != tags.end()) ? it->second.get() : nullptr;
    }
    
    uint8_t getByte(const std::string& key) const {
        auto tag = get(key);
        if (tag && tag->getType() == Type::Byte) {
            return static_cast<const ByteTag*>(tag)->getValue();
        }
        return 0;
    }
    
    const ListTag* getList(const std::string& key) const;
    
    const StringTag* getString(const std::string& key) const {
        auto tag = get(key);
        if (tag && tag->getType() == Type::String) {
            return static_cast<const StringTag*>(tag);
        }
        return nullptr;
    }
};

class ListTag : public Tag {
private:
    std::vector<std::unique_ptr<Tag>> tags;
    Type listType;
public:
    ListTag(Type type) : listType(type) {}
    Type getType() const override { return Type::List; }
    size_t size() const { return tags.size(); }
    
    void add(std::unique_ptr<Tag> tag) {
        if (tag && tag->getType() == listType) {
            tags.push_back(std::move(tag));
        }
    }
    
    const Tag* get(size_t index) const {
        if (index < tags.size()) {
            return tags[index].get();
        }
        return nullptr;
    }
    
    const CompoundTag* getCompound(size_t index) const;
};

// Inline implementations after full class definitions
inline const ListTag* CompoundTag::getList(const std::string& key) const {
    auto tag = get(key);
    if (tag && tag->getType() == Type::List) {
        return static_cast<const ListTag*>(tag);
    }
    return nullptr;
}

inline const CompoundTag* ListTag::getCompound(size_t index) const {
    const Tag* tag = get(index);
    if (tag && tag->getType() == Type::Compound) {
        return static_cast<const CompoundTag*>(tag);
    }
    return nullptr;
}