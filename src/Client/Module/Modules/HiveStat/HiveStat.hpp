#pragma once

#include "../Module.hpp"
#include "../Misc/HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "../../../../Utils/Hive.hpp"
#include "Assets/Assets.hpp"
#include "Events/Render/RenderEvent.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <unordered_set>


class HiveStat : public Module
{
    std::vector<std::pair<std::string, Hive::PlayerStats>> playerStatsList;
    std::vector<std::string> queueList;
    std::shared_mutex queueMutex;
    std::condition_variable_any queueCondition;
    std::thread fetchThread;
    bool stopThread = false;
    bool renderOverlay = true;
    Vec2<float> currentPos;
    bool shouldReset = false;
    std::map<std::string, std::thread> playerThreads;
    std::vector<Hive::LeaderboardEntry> leaderboardEntries;
    std::mutex leaderboardMutex;
    std::atomic<bool> leaderboardFetchInProgress = false;
    std::chrono::steady_clock::time_point leaderboardLastFetch = std::chrono::steady_clock::time_point::min();
    std::string leaderboardGameId;
    bool leaderboardMonthly = false;
    int leaderboardCode = 0;

    // Leaderboard separation
    Vec2<float> leaderboardPos;

    const std::unordered_map<std::string, ImU32> colorMap = {
        {"dark_blue", IM_COL32(38, 93, 255, 255)},
        {"red", IM_COL32(255, 102, 102, 255)},
        {"green", IM_COL32(76, 217, 100, 255)},
        {"gold", IM_COL32(255, 183, 77, 255)},
        {"gray", IM_COL32(160, 160, 160, 255)},
        {"magenta", IM_COL32(213, 128, 255, 255)},
        {"aqua", IM_COL32(77, 208, 225, 255)},
        {"yellow", IM_COL32(255, 213, 79, 255)},
        {"dark_red", IM_COL32(176, 0, 32, 255)},
        {"light_gray", IM_COL32(192, 192, 192, 255)},
        {"olive", IM_COL32(255, 241, 118, 255)},
        {"orange", IM_COL32(255, 138, 80, 255)},
        {"dark_aqua", IM_COL32(125, 232, 242, 255)},
    };

    // Minecraft formatting code -> team color (includes Hive-specific codes)
    const std::unordered_map<char, ImU32> teamColors = {
        {'0', IM_COL32(0, 0, 0, 255)},         // black
        {'1', IM_COL32(0, 0, 170, 255)},        // dark_blue
        {'2', IM_COL32(0, 170, 0, 255)},        // dark_green
        {'3', IM_COL32(0, 170, 170, 255)},       // dark_aqua
        {'4', IM_COL32(170, 0, 0, 255)},         // dark_red
        {'5', IM_COL32(170, 0, 170, 255)},       // dark_purple
        {'6', IM_COL32(255, 170, 0, 255)},       // gold
        {'7', IM_COL32(170, 170, 170, 255)},     // gray
        {'8', IM_COL32(85, 85, 85, 255)},        // dark_gray
        {'9', IM_COL32(85, 85, 255, 255)},       // blue
        {'a', IM_COL32(85, 255, 85, 255)},       // green
        {'b', IM_COL32(85, 255, 255, 255)},      // aqua
        {'c', IM_COL32(255, 85, 85, 255)},       // red
        {'d', IM_COL32(255, 85, 255, 255)},      // light_purple
        {'e', IM_COL32(255, 255, 85, 255)},      // yellow
        {'f', IM_COL32(255, 255, 255, 255)},     // white
        {'p', IM_COL32(82, 255, 25, 255)},       // light_green (hive custom)
        {'s', IM_COL32(246, 140, 10, 255)},      // orange (hive custom)
        {'i', IM_COL32(211, 96, 255, 255)},      // hive ultimate
        {'j', IM_COL32(84, 255, 255, 255)},      // light_blue (hive custom)
    };

    // Team color cache: player clean name -> team formatting code character
    std::shared_mutex playerTeamMutex;
    std::unordered_map<std::string, char> playerTeamCache;

    // Highlight entries: per-player custom colors
    int totalHighlights = 0;

public:
    HiveStat() : Module("Hive Statistics", "Show players statistics in game",
                        IDR_HIVE_STATS_PNG, "O", false, {"stats", "kd", "overlay"})
    {
    };

    void onSetup() override;

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void fetchPlayerStats(const std::string& playerName);
    void fetchLeaderboard(const std::string& gameId, bool monthly);

    void onRender(RenderEvent& event);

    void onPacketReceive(PacketEvent& event);

    void onKey(const KeyEvent& event);

    // Helpers
    static char extractTeamChar(const std::string& rawName);
    ImU32 getHighlightColor(const std::string& playerName);
};
