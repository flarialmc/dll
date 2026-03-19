#pragma once

#include <cstdint>
#include <string>
#include <functional>

class HashedString {
public:
    uint64_t hash{};
    std::string text;
    mutable const HashedString *lastMatch{};

    HashedString() = default;

    explicit HashedString(const std::string &text) : text(text) {
        this->computeHash();
    }

    explicit HashedString(const char* text) : text(text) {
        this->computeHash();
    }

    void computeHash() {
        const uint64_t FNV_OFFSET_BASIS = 0xCBF29CE484222325u;
        const uint64_t FNV_PRIME = 0x100000001B3u;

        uint64_t _hash = FNV_OFFSET_BASIS;
        if (!this->text.empty()) {
            for (char c : this->text) {
                _hash *= FNV_PRIME;
                _hash ^= c;
            }
        } else {
            _hash = 0;
        }

        this->hash = _hash;
    }

    [[nodiscard]] uint64_t getHash() const { return hash; }
    [[nodiscard]] const std::string& getString() const { return text; }
    [[nodiscard]] bool empty() const { return text.empty(); }

    bool operator==(const HashedString &other) const {
        if (this == &other || lastMatch == &other) return true;
        if (hash != other.hash) return false;
        if (text == other.text) {
            lastMatch = &other;
            other.lastMatch = this;
            return true;
        }
        return false;
    }

    bool operator!=(const HashedString &other) const {
        return !(*this == other);
    }

    bool operator<(const HashedString &other) const {
        if (hash < other.hash) return true;
        if (hash > other.hash) return false;
        return text < other.text;
    }
};

namespace std {
    template<>
    struct hash<HashedString> {
        size_t operator()(const HashedString &hs) const noexcept {
            return static_cast<size_t>(hs.getHash());
        }
    };
}
