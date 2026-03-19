#pragma once

#include <string>
#include <unordered_map>
#include "../Utils/Utils.hpp"

#define DEPRECATE_SIG(name) \
    []{ \
        Mgr.removeSignature(Utils::hash(name)); \
    }()

#define ADD_SIG(name, sig) \
    []{ \
        Mgr.addSignature(Utils::hash(name), sig, name); \
    }()

#define GET_SIG(name) \
    []{ \
        return Mgr.getSig(Utils::hash(name)); \
    }()

#define GET_SIG_ADDRESS(name) \
    []{ \
        return Mgr.getSigAddress(Utils::hash(name)); \
    }()

#define ADD_OFFSET(name, offset) \
    []{ \
        Mgr.addOffset(Utils::hash(name), offset); \
    }()

#define GET_OFFSET(name) \
    []{ \
        return Mgr.getOffset(Utils::hash(name)); \
    }()

class SignatureAndOffsetManager {
public:
    void addSignature(unsigned int hash, const char* sig, const char* name);
    void removeSignature(unsigned int hash);
    [[nodiscard]] const char* getSig(unsigned int hash) const;
    [[nodiscard]] const char* getSigName(unsigned int hash) const;
    [[nodiscard]] uintptr_t getSigAddress(unsigned int hash) const;

    void addOffset(unsigned int hash, int offset);
    [[nodiscard]] int getOffset(unsigned int hash) const;

    void scanAllSignatures();

    void clear();

private:
    struct SignatureData {
        std::string signature;
        std::string name;
        uintptr_t address;
    };
    std::unordered_map<unsigned int, SignatureData> sigs{};
    std::unordered_map<unsigned int, int> offsets{};
};

extern SignatureAndOffsetManager Mgr;
