#pragma once

#include <Command/Command.hpp>

class UnbindCommand : public Command {
public:
	UnbindCommand() : Command("unbind", "Unbinds a module.") {}
	void execute(const std::vector<std::string>& args) override;
};