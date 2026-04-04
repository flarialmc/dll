#pragma once
#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "Assets/Assets.hpp"
#include "Utils/HiveMaps.hpp"
#include <map>
#include <chrono>
class HiveUtils : public Module {
private:
	bool triggered = false;
	int deaths = 0;
	std::string teamcolor = "THIS IS NEEDED TRUST!!!!!!!!!!!!!!";
	std::string Copy_CS = "THIS IS NEEDED EVEN MORE TRUST!!!!";
	bool mapsFetched = false;
	std::string lastFetchedGame;
	std::vector<HiveMaps::MapInfo> currentGameMaps;
	std::string selectedGameForPrefs = "bed";
	bool wasPrefsRefreshing = false;
	static std::string getMapPrefsKey(const std::string& gameId) {
		std::string key = gameId;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		return "mapvote_prefs_" + key;
	}
	static std::string getAvoidedMapsKey(const std::string& gameId) {
		std::string key = gameId;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		return "mapavoid_" + key;
	}
	std::string selectedGameForAvoider = "bed";
	std::vector<HiveMaps::MapInfo> currentAvoiderMaps;
	std::string lastFetchedAvoiderGame;
	bool wasAvoiderRefreshing = false;
	std::optional<std::pair<uint32_t, std::vector<std::string>>> parseMapVoteModal(const std::string& formJson);
	std::optional<int> findPreferredMapIndex(const std::vector<std::string>& modalMaps, const std::string& currentGame);
	void sendModalResponse(uint32_t formId, int buttonIndex);
	std::chrono::steady_clock::time_point playerAvoiderLastRequeue;
	bool playerAvoiderLastRequeueSet = false;
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
	void refreshMapsForGame(const std::string& gameId);
	void updatePrefsMapsFromCache(const std::string& gameId);
	std::vector<std::string> getMapPreferences(const std::string& gameId);
	void setMapPreferences(const std::string& gameId, const std::vector<std::string>& maps);
	void moveMapPreference(const std::string& gameId, int fromIndex, int toIndex);
	void refreshMapsForAvoider(const std::string& gameId);
	void updateAvoiderMapsFromCache(const std::string& gameId);
	std::vector<std::string> getAvoidedMaps(const std::string& gameId);
	void setAvoidedMaps(const std::string& gameId, const std::vector<std::string>& maps);
	void addAvoidedMap(const std::string& gameId, const std::string& mapName);
	void removeAvoidedMap(const std::string& gameId, const std::string& mapName);
};
