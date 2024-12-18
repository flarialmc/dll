#pragma once

#include <Command/Command.hpp>

class TestCommand : public Command {
public:
    TestCommand() : Command("test", "Test stuff") {};
    void execute(const std::vector<std::string>& args) override;
};