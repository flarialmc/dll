#pragma once

#include <string>
#include <unordered_map>
#include "../Utils/Utils.hpp"

#define DEPRECATE_SIG(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        Mgr.removeSignature(hash_val); \
    }()

#define ADD_SIG(name, sig) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        Mgr.addSignature(hash_val, sig); \
    }()

#define GET_SIG(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        static auto sig = Mgr.getSig(hash_val);\
        return sig; \
    }()

#define GET_SIG_ADDRESS(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        static auto sig = Mgr.getSigAddress(hash_val);\
        return sig; \
    }()

#define ADD_OFFSET(name, offset) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        Mgr.addOffset(hash_val, offset); \
    }()

#define GET_OFFSET(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        static auto offset = Mgr.getOffset(hash_val);\
        return offset; \
    }()

class SignatureAndOffsetManager {
public:
    void addSignature(unsigned int hash, const char* sig);
    void removeSignature(unsigned int hash);
    [[nodiscard]] const char* getSig(unsigned int hash) const;
    [[nodiscard]] uintptr_t getSigAddress(unsigned int hash) const;

    void addOffset(unsigned int hash, int offset);
    [[nodiscard]] int getOffset(unsigned int hash) const;

    void scanAllSignatures();

    void clear();

private:
    std::unordered_map<unsigned int, std::pair<std::string, uintptr_t>> sigs{};
    std::unordered_map<unsigned int, int> offsets{};
};

extern SignatureAndOffsetManager Mgr;
