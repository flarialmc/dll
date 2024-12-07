#include "LuaCommand.hpp"

#include "../../../Scripting/Scripting.hpp"

void LuaCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .lua <reload/path>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "path") {
        Utils::openSubFolder("Flarial\\scripts");
    }
    if (action == "reload") {
        // nikita u do this
        addCommandMessage("§aReloading scripts...");
    }
}
