#include "BindCommand.hpp"

void BindCommand::execute(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        addCommandMessage("§cUsage: .bind <module> <key>");
        return;
    }
}
