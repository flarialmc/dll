#pragma once

#include <Command/Command.hpp>

class PathCommand : public Command {
private:

    void open(std::string folder = "");

public:

    PathCommand() : Command("path", "Opens the client directory in File Explorer.") {}

    void execute(const std::vector<std::string>& args) override;

};
