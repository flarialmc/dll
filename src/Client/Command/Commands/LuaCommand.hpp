#pragma once

#include "../Command.hpp"

class LuaCommand : public Command {
public:
    LuaCommand() : Command("lua", "Manages scripts") {}
    void execute(const std::vector<std::string>& args) override;
};