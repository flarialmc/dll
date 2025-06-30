#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <vector>
#include <algorithm>
#include "Events/EventManager.hpp"

class TabList : public Module {

public:


    TabList();;

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    // Helper function to assign priority based on role using ApiUtils
    static int getRolePriority(const std::string& name);

    // Helper function to sort players by Flarial hierarchy
    static std::vector<const std::pair<const mcUUID, PlayerListEntry>*> sortByFlarialHierarchy(const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap);

    // Updated function to handle Flarial First in alphabetical order
    static std::vector<const std::pair<const mcUUID, PlayerListEntry> *> copyMapInAlphabeticalOrder(const std::unordered_map<mcUUID, PlayerListEntry>& sourceMap, bool flarialFirst);

    int getPingImage(int ping);

    void normalRender(int index, std::string& value) override;

    void onRender(RenderEvent& event);

    void onMouse(const MouseEvent& event);

    void onKey(const KeyEvent& event);
};