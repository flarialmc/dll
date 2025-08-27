#pragma once

#include <Command/Command.hpp>

class LuaCommand : public Command, public std::enable_shared_from_this<LuaCommand> {
public:
    LuaCommand() : Command("lua", "Manages scripts.") {}
    void execute(const std::vector<std::string>& args) override;
};