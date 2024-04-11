#include "SignatureAndOffsetManager.hpp"

SignatureAndOffsetManager Mgr;

void SignatureAndOffsetManager::addSignature(const char* name, const char* sig) {
    sigs[Utils::hash(name)] = sig;
}

const char* SignatureAndOffsetManager::getSig(const char* name) const {
    auto it = sigs.find(Utils::hash(name));
    return it != sigs.end() ? it->second.c_str() : nullptr;
}

void SignatureAndOffsetManager::addOffset(const char* name, int offset) {
    offsets[Utils::hash(name)] = offset;
}

int SignatureAndOffsetManager::getOffset(const char* name) const {
    auto it = offsets.find(Utils::hash(name));
    return it != offsets.end() ? it->second : 0; // Default to 0 if not found
}

void SignatureAndOffsetManager::clear() {
    sigs.clear();
    offsets.clear();
}