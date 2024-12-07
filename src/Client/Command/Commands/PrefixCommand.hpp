#pragma once

#include "../Command.hpp"

class PrefixCommand : public Command {
public:
    PrefixCommand() : Command("prefix", "What prefix triggers commands") {}
    void execute(const std::vector<std::string>& args) override;
};