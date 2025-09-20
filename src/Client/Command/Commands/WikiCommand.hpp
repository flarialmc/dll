#pragma once

#include <Command/Command.hpp>


class WikiCommand : public Command {

public:

	WikiCommand() : Command("wiki", "Opens the minecraft.wiki page for selected item.") {}

	void execute(const std::vector<std::string>& args) override;

};