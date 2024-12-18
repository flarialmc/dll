#include "PrefixCommand.hpp"

void PrefixCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .prefix <key>");
        return;
    }

    if (args[0].length() != 1) {
        addCommandMessage("§cYour prefix must be 1 character long.");
        return;
    }

    prefix = args[0];
    addCommandMessage("§aYour prefix has been set to §b{}", prefix);
}
