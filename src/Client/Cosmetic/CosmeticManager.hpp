#pragma once

#include "Cosmetics/Cosmetic.hpp"
#include <map>
#include <memory>
#include <string>

namespace CosmeticManager {
    extern inline std::map<size_t, std::shared_ptr<Cosmetic>> CosmeticMap = {};

    template<typename T, typename... ArgsT>
    void addCosmetic(ArgsT... args) {
        auto cosmeticPtr = std::make_shared<T>(args...);
        size_t hash = std::hash<std::string>{}(cosmeticPtr->name);
        CosmeticMap[hash] = cosmeticPtr;
    }

    void Initialize();
};
