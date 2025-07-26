#pragma once
#include <Command/Command.hpp>


class ConfigCommand : public Command {
public:
    ConfigCommand() : Command("config", "Helps configure flarial.") {}
    void execute(const std::vector<std::string>& args) override;

};
