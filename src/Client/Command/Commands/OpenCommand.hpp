#pragma once

#include <Command/Command.hpp>

class OpenCommand : public Command {
  public:
    OpenCommand() : Command("open", "Opens the ClickGUI") {};
    void execute(const std::vector<std::string>& args) override;
};
