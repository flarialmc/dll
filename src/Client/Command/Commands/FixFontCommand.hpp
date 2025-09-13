#pragma once

#include <Command/Command.hpp>

class FixFontCommand : public Command {
public:
	FixFontCommand() : Command("fixfont", "Disables the \'Override Font Weight\' setting and sets the font weight back to normal, fixes most invisible font issues.") {}
	void execute(const std::vector<std::string>& args) override;
};
