#pragma once

#include "../Module.hpp"
#include "Events/Network/PacketEvent.hpp"
#include "Events/Game/ChatScreenControllerTickEvent.hpp"


class CompactChat : public Module {

private:

    bool refreshChat = false;
    std::vector<std::string> messageVec;
    std::unordered_map<std::string, int> counts;
    int prevMsgVecSize = -1;
    bool donext = false;

public:

    CompactChat() : Module("Compact Chat", "Stacks duplicate messages in chat.",
        IDR_COMPACTCHAT_PNG, "") {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    std::string getCountText(int count);

    void replace(std::string& str, std::string from, std::string to);

    void onPacketReceive(PacketEvent& event);

    void onChatScreenControllerTickEvent(ChatScreenControllerTickEvent& event);

};

