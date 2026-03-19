#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <optional>
#include <chrono>
#include <json/json.hpp>

namespace HiveMaps {

    // Map data from the API :3
    struct MapInfo {
        std::string name;
        std::string season;    // NO_SEASON, WINTERFEST, HALLOWEEN, SPRING, etc.
        std::string variant;   // REGULAR, DUOS, SQUADS, MEGA, etc.
        std::string imageUrl;
    };

    // Cache entry with timestamp
    struct CacheEntry {
        std::vector<MapInfo> maps;
        std::chrono::steady_clock::time_point timestamp;
    };

    // Game ID mapping from HiveModeCatcher format to API format
    // e.g., "BED" -> "bed", "SKY" -> "sky"
    inline std::string gameIdToApiFormat(const std::string& gameId) {
        std::string lower = gameId;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        // Handle variants like BED-DUOS -> bed
        size_t dashPos = lower.find('-');
        if (dashPos != std::string::npos) {
            lower = lower.substr(0, dashPos);
        }

        // Map game codes to API identifiers
        static const std::map<std::string, std::string> gameMap = {
            {"bed", "bed"},
            {"sky", "sky"},
            {"wars", "wars"},
            {"dr", "dr"},
            {"hide", "hide"},
            {"sg", "sg"},
            {"murder", "murder"},
            {"ctf", "ctf"},
            {"drop", "drop"},
            {"ground", "ground"},
            {"build", "build"},
            {"party", "party"},
            {"bridge", "bridge"},
            {"grav", "grav"}
        };

        auto it = gameMap.find(lower);
        return it != gameMap.end() ? it->second : lower;
    }

    // Extract variant from game mode (e.g., "BED-DUOS" -> "DUOS", "BED" -> "REGULAR")
    // Maps our settings names to Hive API variant names :3
    inline std::string extractVariant(const std::string& gameId) {
        size_t dashPos = gameId.find('-');
        if (dashPos != std::string::npos) {
            std::string variant = gameId.substr(dashPos + 1);
            // Convert to uppercase for comparison
            std::transform(variant.begin(), variant.end(), variant.begin(), ::toupper);
            // Map "SOLOS" to "REGULAR" since Hive API uses "REGULAR" for solo variants
            if (variant == "SOLOS") {
                return "REGULAR";
            }
            return variant;
        }
        // Default to REGULAR for modes without variant suffix
        return "REGULAR";
    }

    // Check if a map matches the current game variant
    inline bool mapMatchesVariant(const MapInfo& map, const std::string& variant) {
        // Normalize both to uppercase for comparison
        std::string mapVar = map.variant;
        std::string targetVar = variant;
        std::transform(mapVar.begin(), mapVar.end(), mapVar.begin(), ::toupper);
        std::transform(targetVar.begin(), targetVar.end(), targetVar.begin(), ::toupper);

        return mapVar == targetVar;
    }

    // Fetches maps for a game from the Hive API
    // Returns empty vector on failure
    std::vector<MapInfo> fetchMapsFromApi(const std::string& gameId);

    // Gets maps for a game, using cache if available and not expired
    // Cache expires after 1 hour by default
    std::vector<MapInfo> getMapsForGame(const std::string& gameId, bool forceRefresh = false);

    // Gets maps from cache only (never blocks on network)
    // Returns cached maps or empty vector if no cache exists
    std::vector<MapInfo> getMapsFromCacheOnly(const std::string& gameId);

    // Triggers an async refresh for a game's maps
    // Returns immediately, the cache will be updated in the background
    void refreshMapsAsync(const std::string& gameId);

    // Check if an async refresh is in progress for a game
    bool isRefreshInProgress(const std::string& gameId);

    // Gets maps filtered by variant (e.g., only DUOS maps)
    std::vector<MapInfo> getMapsForGameVariant(const std::string& currentGame);

    // Saves the cache to disk
    void saveCache();

    // Loads the cache from disk
    void loadCache();

    // Gets the cache file path
    std::string getCacheFilePath();

    // Check if a game supports map voting
    bool gameSupportsMapVoting(const std::string& gameId);

    // Get list of all supported games
    std::vector<std::string> getSupportedGames();

} // namespace HiveMaps
