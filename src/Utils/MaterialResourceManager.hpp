#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>


namespace dragon::materials {
    struct MaterialLocation {
        uint64_t mHash;
    };
} // namespace dragon::materials

namespace std {
    template <> class hash<dragon::materials::MaterialLocation> {
    public:
        std::size_t operator()(
            const dragon::materials::MaterialLocation &location) const noexcept {
            return location.mHash;
        }
    };
}; // namespace std

namespace dragon::materials {
    struct MaterialResource {
        std::shared_ptr<void> ptr;
        bool used;
    };

    struct MaterialResourceManager {
        std::unordered_map<MaterialLocation, MaterialResource> mCache;
        std::mutex mMutex;

        void trim() {
            mMutex.lock();
            for (auto it = mCache.begin(); it != mCache.end(); it++) {
                if (it->second.used) {
                    it->second.used = false;
                } else {
                    mCache.erase(it);
                }
            }
            mMutex.unlock();
        }

        void forceTrim() {
            mMutex.lock();
            for (auto it = mCache.begin(); it != mCache.end(); it++) {
                it->second.used = false;
                mCache.erase(it);
            }
            mMutex.unlock();
        }
    };
} // namespace dragon::materials
