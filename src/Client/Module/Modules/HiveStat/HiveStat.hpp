#pragma once

#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "../../../../Utils/Hive.hpp"
#include "Events/Render/RenderEvent.hpp"


class HiveStat : public Module {
private:
	std::vector<std::pair<std::string, Hive::PlayerStats>> playerStatsList;
	std::vector<std::string> queueList;
	std::shared_mutex queueMutex;
	std::condition_variable_any queueCondition;
	std::thread fetchThread;
	bool stopThread = false;
	bool renderOverlay = true;
	Vec2<float> currentPos;
	bool shoudReset = false;
	std::map<std::string, std::thread> playerThreads;
public:
	HiveStat() : Module("Hive Statistics", "Show players statistics in game",
		IDR_HIVE_STATS_PNG, "O") {
		Module::setup();
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void fetchPlayerStats(const std::string& playerName);

	void onRender(RenderEvent& event);

	void onPacketReceive(PacketEvent& event);

	void onKey(KeyEvent& event);
};

