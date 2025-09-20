#pragma once

#include <Command/Command.hpp>

class FixFontCommand : public Command {
public:
	FixFontCommand() : Command("fixfont", "Disables the \'Override Font Weight\' and resets font size and weight, fixes most invisible and enlarged font issues.") {}
	void execute(const std::vector<std::string>& args) override;
};
