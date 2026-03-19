#pragma once

#include <Command/Command.hpp>

class DiagCommand : public Command {
public:
    DiagCommand() : Command("diag", "Dumps diagnostic info to chat and log file.", {"diagnostic", "debug"}) {}
    void execute(const std::vector<std::string>& args) override;
};
