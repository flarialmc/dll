#pragma once

#include <Command/Command.hpp>

class BindCommand : public Command {
public:
    BindCommand() : Command("bind", "Binds a module to a key.") {}
    void execute(const std::vector<std::string>& args) override;
};