#include "SignatureAndOffsetManager.hpp"
#include "../Memory.hpp"

SignatureAndOffsetManager Mgr;

void SignatureAndOffsetManager::addSignature(unsigned int hash, const char* sig) {
    sigs[hash] = { sig, 0 };
}

void SignatureAndOffsetManager::removeSignature(unsigned int hash) {
    sigs.erase(hash);
}

const char* SignatureAndOffsetManager::getSig(unsigned int hash) const {
    auto it = sigs.find(hash);
    return it != sigs.end() ? it->second.first.c_str() : nullptr;
}

uintptr_t SignatureAndOffsetManager::getSigAddress(unsigned int hash) const {
    auto it = sigs.find(hash);
    return it != sigs.end() ? it->second.second : 0;
}

void SignatureAndOffsetManager::addOffset(unsigned int hash, int offset) {
    offsets[hash] = offset;
}

int SignatureAndOffsetManager::getOffset(unsigned int hash) const {
    auto it = offsets.find(hash);
    return it != offsets.end() ? it->second : 0; // Default to 0 if not found
}

void SignatureAndOffsetManager::clear() {
    sigs.clear();
    offsets.clear();
}

void SignatureAndOffsetManager::scanAllSignatures() {
    for (auto& pair : sigs) {
        pair.second.second = Memory::findSig(pair.second.first);
    }
}
