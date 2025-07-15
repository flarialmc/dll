#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <vector>
#include <algorithm>
#include "Events/EventManager.hpp"

class TabList : public Module {
private:
    bool refreshCache = true;

    std::vector<float> columnx;
    std::vector<PlayerListEntry> vecmap;
    float totalWidth;
    float baseTotalHeight;
    size_t validPlayers;

    std::string cache_worldName;
    std::string cache_serverPing;
    std::string cache_playerCount;
    std::string cache_curPlayer;

    ImVec2 worldNameMetrics;
    ImVec2 curPingMetrics;
    ImVec2 playerCountMetrics;
    ImVec2 curPlayerMetrics;

    std::string curPlayer;
    std::string countTxt;
public:


    TabList();

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    // Helper function to assign priority based on role using ApiUtils
    static int getRolePriority(const std::string& name);

    // Helper function to sort players by Flarial hierarchy
    static std::vector<PlayerListEntry> sortByFlarialHierarchy(const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap);

    // Updated function to handle Flarial First in alphabetical order
    static std::vector<PlayerListEntry> copyMapInAlphabeticalOrder(const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap, bool flarialFirst);

    int getPingImage(int ping);

    void onRender(RenderEvent& event);

    void onMouse(const MouseEvent& event);

    void onKey(const KeyEvent& event);
};