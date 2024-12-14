#include "LuaCommand.hpp"

#include <Manager.hpp>
#include <Utils/WinrtUtils.hpp>
#include <Scripting/Scripting.hpp>

void LuaCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .lua <reload/path>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "path") {
        WinrtUtils::openSubFolder("Flarial\\scripts");
    }
    if (action == "reload") {
        Scripting::unloadModules();
        Scripting::loadModules();
        addCommandMessage("§aReloaded all scripts!");
    }
}
