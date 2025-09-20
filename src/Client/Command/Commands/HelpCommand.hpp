#pragma once

#include <Command/Command.hpp>

class HelpCommand : public Command {
public:
    HelpCommand() : Command("help", "Shows all available commands and their usage.") {}
    void execute(const std::vector<std::string>& args) override;
};