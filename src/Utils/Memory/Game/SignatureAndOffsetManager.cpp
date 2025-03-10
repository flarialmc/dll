#include "SignatureAndOffsetManager.hpp"

#include <Utils/Memory/Memory.hpp>

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
    const unsigned int numThreads = std::thread::hardware_concurrency();
    std::vector<std::future<void>> futures;

    size_t totalSize = sigs.size();
    size_t chunkSize = totalSize / numThreads;

    auto it = sigs.begin();
    for (unsigned int i = 0; i < numThreads; ++i) {
        size_t currentChunk = (i == numThreads - 1) ? (totalSize - i * chunkSize) : chunkSize;
        auto endIt = std::next(it, currentChunk);

        futures.push_back(std::async(std::launch::async,
            [this](auto begin, auto end) {
                for (auto iter = begin; iter != end; ++iter) {
                    iter->second.second = Memory::findSig(iter->second.first);
                }
            }, it, endIt));

        it = endIt;
    }

    for (auto& f : futures) {
        f.wait();
    }
}
