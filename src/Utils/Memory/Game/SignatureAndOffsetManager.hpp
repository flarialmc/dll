#pragma once

#include <string>
#include <unordered_map>
#include "../../../Utils/Utils.hpp"

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
    [[nodiscard]] const char* getSig(unsigned int hash) const;

    void addOffset(unsigned int hash, int offset);
    [[nodiscard]] int getOffset(unsigned int hash) const;

    void clear();

private:
    std::unordered_map<unsigned int, std::string> sigs{};
    std::unordered_map<unsigned int, int> offsets{};
};

extern SignatureAndOffsetManager Mgr;
