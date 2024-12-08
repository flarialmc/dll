#pragma once

#include "../../Command.hpp"

class Help : public Command {
public:
    Help() : Command("help", "Shows all available commands and their usage") {}
    void execute(const std::vector<std::string>& args) override;
};