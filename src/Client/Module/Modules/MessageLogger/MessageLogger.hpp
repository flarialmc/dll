#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"



class MessageLogger : public Module {

private:

    bool update = false;
    int prevMsgVecSize = 0;

public:
    MessageLogger(): Module("Message Logger", "Saves chat messages into a file.",
        IDR_COMPACTCHAT_PNG, "", false, {"chat logger"}) {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onPacket(PacketEvent& event);

    void onTick(TickEvent& event);
};