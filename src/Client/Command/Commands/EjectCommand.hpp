#pragma once

#include <Command/Command.hpp>

class EjectCommand : public Command {
public:
    EjectCommand() : Command("eject", "Removes the client from the game.") {}
    void execute(const std::vector<std::string>& args) override;
};