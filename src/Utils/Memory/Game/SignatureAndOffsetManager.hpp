#pragma once

#include <string>
#include <unordered_map>
#include "../../../Utils/Utils.hpp"

#define ADD_SIG(name, sig) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        Mgr.addSignature(name, sig); \
    }()

#define GET_SIG(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        return Mgr.getSig(name); \
    }()

#define ADD_OFFSET(name, offset) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        Mgr.addOffset(name, offset); \
    }()

#define GET_OFFSET(name) \
    []{ \
        constexpr unsigned int hash_val = Utils::hash(name); \
        return Mgr.getOffset(name); \
    }()

class SignatureAndOffsetManager {
public:
    void addSignature(const char* name, const char* sig);
    [[nodiscard]] const char* getSig(const char* name) const;

    void addOffset(const char* name, int offset);
    [[nodiscard]] int getOffset(const char* name) const;

    void clear();

private:
    std::unordered_map<unsigned int, std::string> sigs{};
    std::unordered_map<unsigned int, int> offsets{};
};

extern SignatureAndOffsetManager Mgr;
