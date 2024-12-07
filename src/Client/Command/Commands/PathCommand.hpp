#pragma once

#include "../Command.hpp"

class PathCommand : public Command {
public:
    PathCommand() : Command("path", "Copies the client directory to your clipboard") {}
    void execute(const std::vector<std::string>& args) override;
};
