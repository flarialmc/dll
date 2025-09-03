#pragma once

#include <Command/Command.hpp>

class PrefixCommand : public Command {
public:
    PrefixCommand() : Command("prefix", "Changes the prefix for commands.") {}
    void execute(const std::vector<std::string>& args) override;
};