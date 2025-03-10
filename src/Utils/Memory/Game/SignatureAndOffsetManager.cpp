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
    std::vector<std::thread> threads;
    std::atomic<size_t> index{0};

    auto worker = [this, &index]() {
        while (true) {
            size_t i = index.fetch_add(1, std::memory_order_relaxed);
            if (i >= sigs.size()) break;  // No more work

            auto& sigPair = *(std::next(sigs.begin(), i));
            sigPair.second.second = Memory::findSig(sigPair.second.first);
        }
    };

    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }
}