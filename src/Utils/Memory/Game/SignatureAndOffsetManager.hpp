#pragma once

#include <string>
#include <unordered_map>

// TODO: Signature class

#define ADD_SIG(name, sig) Mgr.addSignature(name, sig)
#define GET_SIG(name) Mgr.getSig(name)

#define ADD_OFFSET(name, offset) Mgr.addOffset(name, offset)
#define GET_OFFSET(name) Mgr.getOffset(name)

class SignatureAndOffsetManager {
public:
    void addSignature(const std::string& name, const std::string& sig);
    [[nodiscard]] std::string getSig(const std::string& name) const;

    void addOffset(const std::string& name, const int& offset);
    [[nodiscard]] int getOffset(const std::string& name) const;

    void clear();

private:
    std::unordered_map<std::string, std::string> sigs{};
    std::unordered_map<std::string, int> offsets{};
};

extern SignatureAndOffsetManager Mgr;
