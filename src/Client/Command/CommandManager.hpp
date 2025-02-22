#pragma once

#include "Command.hpp"

#include "../../Client/Events/EventManager.hpp"

class CommandManager {
public:
    static CommandManager instance;
    static std::vector<std::unique_ptr<Command>> Commands;

    static void initialize();
    static void terminate();
    void onPacket(PacketSendEvent &event);

};