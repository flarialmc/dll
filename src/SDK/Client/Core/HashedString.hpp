#pragma once

#include <cstdint>
#include <string>

class HashedString {
public:
    uint64_t hash{};
    std::string text;
    const HashedString *lastMatch{};

    explicit HashedString(const std::string &text) {
        this->text = text;
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

    bool operator==(HashedString &other) {
        if (this->text == other.text) {
            return ((this->lastMatch == &other) && (other.lastMatch == this));
        }
        return false;
    }

    bool operator!=(HashedString &other) {
        return !(*this == other);
    }

    bool operator<(HashedString &other) const {
        if (this->hash < other.hash) {
            return true;
        }
        if (this->hash <= other.hash) {
            return (strcmp(this->text.c_str(), other.text.c_str()) < 0);
        }
        return false;
    }
};