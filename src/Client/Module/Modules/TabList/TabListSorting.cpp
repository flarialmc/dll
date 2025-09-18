#include "TabList.hpp"
#include "Utils/APIUtils.hpp"

// Player role priority and sorting functions

int TabList::getRolePriority(const std::string &name) {
    std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
    if (clearedName.empty()) return 5; // Lowest priority for invalid names

    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
    if (it == APIUtils::onlineUsers.end()) return 5; // Non-Flarial users

    // Check roles in order of priority using ApiUtils
    if (APIUtils::hasRole("Dev", clearedName)) return 0;
    if (APIUtils::hasRole("Staff", clearedName)) return 1;
    if (APIUtils::hasRole("Gamer", clearedName)) return 2;
    if (APIUtils::hasRole("Supporter", clearedName)) return 3;
    if (APIUtils::hasRole("Booster", clearedName)) return 4;
    return 5; // Default Flarial user (in onlineUsers but no specific role)
}

std::vector<PlayerListEntry> TabList::sortVecmap(
    const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap, bool flarialFirst, bool sort) {
    if (flarialFirst) {
        std::vector<PlayerListEntry> flarialEntries;
        std::vector<PlayerListEntry> nonFlarialEntries;

        for (const auto &pair: sourceMap) {
            const PlayerListEntry &entry = pair.second;
            if (entry.name.empty()) continue;

            std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(entry.name));
            if (clearedName.empty()) clearedName = entry.name;

            auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
            if (it != APIUtils::onlineUsers.end()) {
                flarialEntries.push_back(entry);
            } else {
                nonFlarialEntries.push_back(entry);
            }
        }

        if (sort) {
            auto sortByName = [](const PlayerListEntry &a, const PlayerListEntry &b) {
                std::string clean_a = String::removeColorCodes(a.name);
                std::string clean_b = String::removeColorCodes(b.name);
                return std::lexicographical_compare(clean_a.begin(), clean_a.end(), clean_b.begin(), clean_b.end(),
                                                    [](char c1, char c2) { return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2)); });
            };

            auto sortByRoleAndName = [&sortByName](const PlayerListEntry &a, const PlayerListEntry &b) {
                int priorityA = getRolePriority(a.name);
                int priorityB = getRolePriority(b.name);
                if (priorityA != priorityB) return priorityA < priorityB;
                return sortByName(a, b);
            };

            std::sort(flarialEntries.begin(), flarialEntries.end(), sortByRoleAndName);
            std::sort(nonFlarialEntries.begin(), nonFlarialEntries.end(), sortByName);
        }

        std::vector<PlayerListEntry> result;
        result.reserve(sourceMap.size());
        result.insert(result.end(), flarialEntries.begin(), flarialEntries.end());
        result.insert(result.end(), nonFlarialEntries.begin(), nonFlarialEntries.end());
        return result;
    } else {
        std::vector<PlayerListEntry> result;
        result.reserve(sourceMap.size());
        for (const auto &pair: sourceMap) {
            if (!pair.second.name.empty()) {
                result.push_back(pair.second);
            }
        }

        // Conditionally sort the single list.
        if (sort) {
            auto sortByName = [](const PlayerListEntry &a, const PlayerListEntry &b) {
                std::string clean_a = String::removeColorCodes(a.name);
                std::string clean_b = String::removeColorCodes(b.name);
                return std::lexicographical_compare(clean_a.begin(), clean_a.end(), clean_b.begin(), clean_b.end(),
                                                    [](char c1, char c2) { return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2)); });
            };
            std::sort(result.begin(), result.end(), sortByName);
        }

        return result;
    }
}