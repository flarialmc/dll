#include "ConfigCommand.hpp"

#include "Modules/Misc/ScriptMarketplace/ScriptMarketplace.hpp"

void ConfigCommand::execute(const std::vector<std::string>& args) {
    std::string action = String::toLower(args[0]);

    if (action == "reload") ScriptMarketplace::reloadAllConfigs();
    else if (action == "create" && !args[1].empty()) { Client::createConfig(args[1]); FlarialGUI::Notify("Created " + args[1]); }
    else if (action == "delete" && !args[1].empty()) { Client::deleteConfig(args[1]); FlarialGUI::Notify("Deleted " + args[1]); }
    else if (action == "select" && !args[1].empty()) { if (std::filesystem::exists(Utils::getRoamingPath() + "\\Flarial\\Config\\" + args[1])) {Client::settings.getSettingByName<std::string>("currentConfig")->value = args[1]; FlarialGUI::Notify("Selected " + args[1]);} }
}
