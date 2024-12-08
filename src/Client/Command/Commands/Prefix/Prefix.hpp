#pragma once

#include "../../Command.hpp"

class Prefix : public Command {
public:
    Prefix() : Command("prefix", "What prefix triggers commands") {}
    void execute(const std::vector<std::string>& args) override;
};