#include "Prefix.hpp"
#include "../../../Client.hpp"

void Prefix::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .prefix <key>");
        return;
    }

    if (args.size() > 1) {
        addCommandMessage("§cUsage: .prefix <key>");
        return;
    }

    if (args[0].length() != 1) {
        addCommandMessage("§cYour prefix must be 1 character long.");
        return;
    }
    
    Client::settings.getSettingByName<std::string>("prefix")->value = args[0];
    FlarialGUI::ResetShit();
    ModuleManager::SaveModulesConfig();
    Client::SaveSettings();
    addCommandMessage("§aYour prefix has been set to §b{}", args[0]);
}