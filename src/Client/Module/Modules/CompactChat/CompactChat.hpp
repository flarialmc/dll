#pragma once

#include "../Module.hpp"
#include "Events/Network/PacketEvent.hpp"


class CompactChat : public Module {

private:

    int count = 1;
    std::string mesg;
    std::map<std::string, std::string> textColors;

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

};

