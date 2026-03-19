#pragma once

#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "Assets/Assets.hpp"
#include "Utils/HiveMaps.hpp"
#include <map>


class HiveUtils : public Module {
private:
	bool triggered = false;
	int deaths = 0;
	std::string teamcolor = "THIS IS NEEDED TRUST!!!!!!!!!!!!!!";
	std::string Copy_CS = "THIS IS NEEDED EVEN MORE TRUST!!!!";

	// Auto Map Vote state :3
	bool mapsFetched = false;
	std::string lastFetchedGame;
	std::vector<HiveMaps::MapInfo> currentGameMaps;
	std::string selectedGameForPrefs = "bed"; // Currently selected game in settings UI
	bool wasPrefsRefreshing = false; // Track if we need one more cache read after refresh completes

	// Helper to get map preferences setting key for a game (preserves variant)
	static std::string getMapPrefsKey(const std::string& gameId) {
		// Normalize to lowercase but preserve variant suffix (e.g., "bed-duos" stays as-is)
		std::string key = gameId;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		return "mapvote_prefs_" + key;
	}

	// Helper to get avoided maps setting key for a game (preserves variant)
	static std::string getAvoidedMapsKey(const std::string& gameId) {
		std::string key = gameId;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		return "mapavoid_" + key;
	}

	// Map avoider state
	std::string selectedGameForAvoider = "bed";
	std::vector<HiveMaps::MapInfo> currentAvoiderMaps;
	std::string lastFetchedAvoiderGame;
	bool wasAvoiderRefreshing = false; // Track if we need one more cache read after refresh completes

	// Parse the "Choose map" modal JSON and find the map buttons
	std::optional<std::pair<uint32_t, std::vector<std::string>>> parseMapVoteModal(const std::string& formJson);

	// Find the best map to vote for based on preferences
	std::optional<int> findPreferredMapIndex(const std::vector<std::string>& modalMaps, const std::string& currentGame);

	// Send a modal form response
	void sendModalResponse(uint32_t formId, int buttonIndex);

public:
	HiveUtils() : Module("Hive Utils", "Handy utilities for The Hive partnered server",
		IDR_HIVE_PNG, "", false, {"auto requeue", "auto rq"} ) {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onPacketReceive(PacketEvent& event);

	void reQ();

	void onKey(KeyEvent& event);

	// Auto map vote helpers
	void refreshMapsForGame(const std::string& gameId);
	void updatePrefsMapsFromCache(const std::string& gameId); // Update local maps from cache without triggering new refresh
	std::vector<std::string> getMapPreferences(const std::string& gameId);
	void setMapPreferences(const std::string& gameId, const std::vector<std::string>& maps);
	void moveMapPreference(const std::string& gameId, int fromIndex, int toIndex);

	// Map avoider helpers (uses same map list as auto vote)
	void refreshMapsForAvoider(const std::string& gameId);
	void updateAvoiderMapsFromCache(const std::string& gameId); // Update local maps from cache without triggering new refresh
	std::vector<std::string> getAvoidedMaps(const std::string& gameId);
	void setAvoidedMaps(const std::string& gameId, const std::vector<std::string>& maps);
	void addAvoidedMap(const std::string& gameId, const std::string& mapName);
	void removeAvoidedMap(const std::string& gameId, const std::string& mapName);
};
