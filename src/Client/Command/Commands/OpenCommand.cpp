#include "OpenCommand.hpp"

#include "../../Module/Manager.hpp"

void OpenCommand::execute(const std::vector<std::string> &args) {
    ModuleManager::getModule("clickgui")->setEnabled(true);
}