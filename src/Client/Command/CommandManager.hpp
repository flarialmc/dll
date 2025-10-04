#pragma once

#include "Command.hpp"

#include "Events/Network/PacketSendEvent.hpp"
#include "Events/Events.hpp"

class CommandManager {
public:
    static CommandManager instance;
    static std::vector<std::shared_ptr<Command>> Commands;

    static void initialize();
    static void terminate();
    void onPacket(PacketSendEvent &event);
};
