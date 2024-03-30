#include "SignatureAndOffsetManager.hpp"
#include "../../Logger/Logger.hpp"

SignatureAndOffsetManager Mgr;

void SignatureAndOffsetManager::addSignature(const std::string& name, const std::string& sig) {
    sigs.emplace(name, sig);
}

std::string SignatureAndOffsetManager::getSig(const std::string& name) const {
    if (!sigs.contains(name)) {
#ifndef NDEBUG
        Logger::debug("[Signatures] Couldn't find sig: " + name);
#endif
        return "";
    }
    return sigs.at(name);
}

void SignatureAndOffsetManager::addOffset(const std::string &name, const int& offset) {
    offsets.emplace(name, offset);
}

int SignatureAndOffsetManager::getOffset(const std::string& name) const {
    if (!offsets.contains(name)) {
        #ifndef NDEBUG
        Logger::debug("[Offsets] Couldn't find offset: " + name);
        #endif
        return 0;
    }
    return offsets.at(name);
}

void SignatureAndOffsetManager::clear() {
    sigs.clear();
}

