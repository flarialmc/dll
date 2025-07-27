#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"


class MessageLogger : public Module {
private:
    int prevMsgVecSize = 0;
public:
    MessageLogger();

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    // void settingsRender(float settingsOffset) override;

    void onPacket(PacketEvent& event);
};