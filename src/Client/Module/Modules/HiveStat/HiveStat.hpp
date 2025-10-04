#pragma once

#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "../../../../Utils/Hive.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Events.hpp"


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

	const std::unordered_map<std::string, ImU32> colorMap = {
		{"dark_blue",   IM_COL32(38, 93, 255, 255)},
		{"red",         IM_COL32(255, 102, 102, 255)},
		{"green",       IM_COL32(76, 217, 100, 255)},
		{"gold",        IM_COL32(255, 183, 77, 255)},
		{"gray",        IM_COL32(160, 160, 160, 255)},
		{"magenta",     IM_COL32(213, 128, 255, 255)},
		{"aqua",        IM_COL32(77, 208, 225, 255)},
		{"yellow",      IM_COL32(255, 213, 79, 255)},
		{"dark_red",    IM_COL32(176, 0, 32, 255)},
		{"light_gray",  IM_COL32(192, 192, 192, 255)},
		{"olive",       IM_COL32(255, 241, 118, 255)},
		{"orange",      IM_COL32(255, 138, 80, 255)},
		{"dark_aqua",   IM_COL32(125, 232, 242, 255)},
	};

public:
	HiveStat() : Module("Hive Statistics", "Show players statistics in game",
		IDR_HIVE_STATS_PNG, "O", false, {"stats", "kd", "overlay"}) {
		
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

