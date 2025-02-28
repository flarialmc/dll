#pragma once

#include <Command/Command.hpp>

class SkinStealCommand : public Command {
public:
    SkinStealCommand() : Command("steal", "Steal the skin of a player.") {};
    void execute(const std::vector<std::string>& args) override;
};