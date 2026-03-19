#include "HiveMaps.hpp"
#include "APIUtils.hpp"
#include "Utils.hpp"
#include "Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <set>
#include <thread>

namespace HiveMaps {

    // Cache storage
    static std::map<std::string, CacheEntry> mapCache;
    static std::mutex cacheMutex;
    static constexpr auto CACHE_EXPIRY = std::chrono::hours(1);
    static bool cacheLoaded = false;

    // Track which games have async refreshes in progress
    static std::set<std::string> refreshesInProgress;
    static std::mutex refreshMutex;

    std::string getCacheFilePath() {
        return Utils::getRoamingPath() + "\\Flarial\\hive_maps_cache.json";
    }

    std::vector<MapInfo> fetchMapsFromApi(const std::string& gameId) {
        std::vector<MapInfo> result;

        std::string apiGameId = gameIdToApiFormat(gameId);
        std::string url = "https://api.playhive.com/v0/game/map/" + apiGameId;

        Logger::info("Fetching maps from Hive API: {}", url);

        try {
            auto [statusCode, response] = APIUtils::Request(url, "GET");

            if (statusCode != 200) {
                Logger::warn("Hive API returned status {}: {}", statusCode, response);
                return result;
            }

            if (!nlohmann::json::accept(response)) {
                Logger::warn("Invalid JSON response from Hive API");
                return result;
            }

            auto json = nlohmann::json::parse(response);

            if (!json.is_array()) {
                Logger::warn("Unexpected JSON format from Hive API (expected array)");
                return result;
            }

            for (const auto& mapJson : json) {
                MapInfo map;

                if (mapJson.contains("name") && mapJson["name"].is_string()) {
                    map.name = mapJson["name"].get<std::string>();
                } else {
                    continue; // Skip maps without names
                }

                if (mapJson.contains("season") && mapJson["season"].is_string()) {
                    map.season = mapJson["season"].get<std::string>();
                } else {
                    map.season = "NO_SEASON";
                }

                if (mapJson.contains("variant") && mapJson["variant"].is_string()) {
                    map.variant = mapJson["variant"].get<std::string>();
                } else {
                    map.variant = "REGULAR";
                }

                if (mapJson.contains("image") && mapJson["image"].is_string()) {
                    map.imageUrl = mapJson["image"].get<std::string>();
                }

                result.push_back(std::move(map));
            }

            Logger::info("Fetched {} maps for game {}", result.size(), apiGameId);

        } catch (const std::exception& e) {
            Logger::error("Error fetching maps from Hive API: {}", e.what());
        }

        return result;
    }

    std::vector<MapInfo> getMapsForGame(const std::string& gameId, bool forceRefresh) {
        std::string apiGameId = gameIdToApiFormat(gameId);

        // Load cache from disk if not loaded yet
        if (!cacheLoaded) {
            loadCache();
            cacheLoaded = true;
        }

        std::lock_guard<std::mutex> lock(cacheMutex);

        auto now = std::chrono::steady_clock::now();

        // Check if we have a valid cached entry
        auto it = mapCache.find(apiGameId);
        if (!forceRefresh && it != mapCache.end()) {
            auto age = now - it->second.timestamp;
            if (age < CACHE_EXPIRY && !it->second.maps.empty()) {
                return it->second.maps;
            }
        }

        // Fetch fresh data from API
        auto maps = fetchMapsFromApi(apiGameId);

        if (!maps.empty()) {
            mapCache[apiGameId] = {maps, now};
            // Save cache to disk asynchronously would be nice, but for now just save
            // (Could use std::async here for non-blocking save)
            saveCache();
        } else if (it != mapCache.end() && !it->second.maps.empty()) {
            // If API failed but we have cached data, use it even if expired
            Logger::warn("Using expired cache for game {} (API unavailable)", apiGameId);
            return it->second.maps;
        }

        return maps;
    }

    std::vector<MapInfo> getMapsFromCacheOnly(const std::string& gameId) {
        std::string apiGameId = gameIdToApiFormat(gameId);

        // Load cache from disk if not loaded yet
        if (!cacheLoaded) {
            loadCache();
            cacheLoaded = true;
        }

        std::lock_guard<std::mutex> lock(cacheMutex);

        auto it = mapCache.find(apiGameId);
        if (it != mapCache.end() && !it->second.maps.empty()) {
            return it->second.maps;
        }

        return {};
    }

    void refreshMapsAsync(const std::string& gameId) {
        std::string apiGameId = gameIdToApiFormat(gameId);

        // Check if refresh is already in progress
        {
            std::lock_guard<std::mutex> lock(refreshMutex);
            if (refreshesInProgress.contains(apiGameId)) {
                return; // Already refreshing
            }
            refreshesInProgress.insert(apiGameId);
        }

        // Start background thread for fetching
        std::thread([apiGameId]() {
            auto maps = fetchMapsFromApi(apiGameId);

            if (!maps.empty()) {
                std::lock_guard<std::mutex> lock(cacheMutex);
                mapCache[apiGameId] = {maps, std::chrono::steady_clock::now()};
                saveCache();
            }

            // Clear the in-progress flag
            {
                std::lock_guard<std::mutex> lock(refreshMutex);
                refreshesInProgress.erase(apiGameId);
            }
        }).detach();
    }

    bool isRefreshInProgress(const std::string& gameId) {
        std::string apiGameId = gameIdToApiFormat(gameId);
        std::lock_guard<std::mutex> lock(refreshMutex);
        return refreshesInProgress.contains(apiGameId);
    }

    std::vector<MapInfo> getMapsForGameVariant(const std::string& currentGame) {
        std::string variant = extractVariant(currentGame);
        auto allMaps = getMapsForGame(currentGame);

        std::vector<MapInfo> filtered;
        for (const auto& map : allMaps) {
            if (mapMatchesVariant(map, variant)) {
                filtered.push_back(map);
            }
        }

        // Remove duplicates (same map name, different seasons)
        // We want unique map names since the vote modal just shows names
        std::vector<MapInfo> unique;
        std::set<std::string> seenNames;
        for (const auto& map : filtered) {
            std::string lowerName = map.name;
            std::ranges::transform(lowerName, lowerName.begin(), ::tolower);
            if (!seenNames.contains(lowerName)) {
                seenNames.insert(lowerName);
                unique.push_back(map);
            }
        }

        return unique;
    }

    void saveCache() {
        try {
            nlohmann::json cacheJson;

            for (const auto& [gameId, entry] : mapCache) {
                nlohmann::json mapsJson = nlohmann::json::array();
                for (const auto& [name, season, variant, imageUrl] : entry.maps) {
                    mapsJson.push_back({
                        {"name", name},
                        {"season", season},
                        {"variant", variant},
                        {"imageUrl", imageUrl}
                    });
                }

                cacheJson[gameId] = {
                    {"maps", mapsJson},
                    {"timestamp", std::chrono::duration_cast<std::chrono::seconds>(
                        entry.timestamp.time_since_epoch()).count()}
                };
            }

            std::string path = getCacheFilePath();
            std::filesystem::create_directories(std::filesystem::path(path).parent_path());

            std::ofstream file(path);
            if (file.is_open()) {
                file << cacheJson.dump(2);
                file.close();
            }

        } catch (const std::exception& e) {
            Logger::error("Error saving map cache: {}", e.what());
        }
    }

    void loadCache() {
        try {
            std::string path = getCacheFilePath();

            if (!std::filesystem::exists(path)) {
                return;
            }

            std::ifstream file(path);
            if (!file.is_open()) {
                return;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();

            if (!nlohmann::json::accept(content)) {
                Logger::warn("Invalid map cache JSON, ignoring");
                return;
            }

            auto cacheJson = nlohmann::json::parse(content);

            std::lock_guard<std::mutex> lock(cacheMutex);

            for (auto& [gameId, entryJson] : cacheJson.items()) {
                CacheEntry entry;

                if (entryJson.contains("maps") && entryJson["maps"].is_array()) {
                    for (const auto& mapJson : entryJson["maps"]) {
                        MapInfo map;
                        if (mapJson.contains("name")) map.name = mapJson["name"].get<std::string>();
                        if (mapJson.contains("season")) map.season = mapJson["season"].get<std::string>();
                        if (mapJson.contains("variant")) map.variant = mapJson["variant"].get<std::string>();
                        if (mapJson.contains("imageUrl")) map.imageUrl = mapJson["imageUrl"].get<std::string>();
                        entry.maps.push_back(std::move(map));
                    }
                }

                entry.timestamp = std::chrono::steady_clock::now() - CACHE_EXPIRY / 2;

                mapCache[gameId] = std::move(entry);
            }

            Logger::info("Loaded map cache with {} games", mapCache.size());

        } catch (const std::exception& e) {
            Logger::error("Error loading map cache: {}", e.what());
        }
    }

    bool gameSupportsMapVoting(const std::string& gameId) {
        static const std::set<std::string> supportedGames = {
            "bed", "sky", "wars", "dr", "hide", "sg", "murder",
            "ctf", "drop", "ground", "build", "bridge", "grav"
        };

        std::string apiGameId = gameIdToApiFormat(gameId);
        return supportedGames.contains(apiGameId);
    }

    std::vector<std::string> getSupportedGames() {
        return {
            "bed", "sky", "wars", "dr", "hide", "sg", "murder",
            "ctf", "drop", "ground", "build", "bridge", "grav"
        };
    }

} // namespace HiveMaps
