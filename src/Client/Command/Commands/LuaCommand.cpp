#include "LuaCommand.hpp"

#include "../../../Scripting/Scripting.hpp"
#include "imgui.h"

void LuaCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        addCommandMessage("§cUsage: .lua <reload/path>");
        return;
    }

    std::string action = String::toLower(args[0]);

    if (action == "path") {
        //TODO: Use winrt to open folder automatically
        ImGui::SetClipboardText((Utils::getClientPath() + "\\scripts").c_str());
        addCommandMessage("§aCopied scripts path to clipboard. Use Win + R and paste to open.");
    }
    if (action == "reload") {
        // nikita u do this
        addCommandMessage("§aReloading scripts...");
    }
}
