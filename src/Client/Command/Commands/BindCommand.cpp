#include "BindCommand.hpp"
#include "../../Module/Manager.hpp"
#include <regex>

void BindCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .bind list / .bind <key(s)> / .bind <module> <key(s)>");
        return;
    }

    if (args[0] == "list") {
        std::string modList;
        int index = 1;
        for (std::shared_ptr<Module> mod : ModuleManager::moduleMap | std::views::values) {
            SettingType<std::string>* setting = mod->settings.getSettingByName<std::string>("keybind");
            if (setting != nullptr && setting->value != "") {
                modList += std::format("\n{}. {}", index, std::regex_replace(mod->name, std::regex(" "), "_"));
                index++;
            }
        }
        addCommandMessage("Keybindable Module List: (case-sensitive)" + modList);
        return;
    }

    std::string keys;
    std::string modName;
    if (args.size() == 1) {
        keys = String::toUpper(args[0]);
        modName = "ClickGUI";
    }
    else {
        keys = String::toUpper(args[1]);
        modName = std::regex_replace(args[0], std::regex("_"), " ");
    }

    std::vector<int> keyInts = Utils::getStringAsKeys(keys);
    if (keyInts.empty() || std::ranges::find(keyInts, -1) != keyInts.end()) {
        if (args.size() == 1) addCommandMessage("Invalid Key(s). §cUsage: .bind <key(s)>");
        else addCommandMessage("Invalid Key(s). §cUsage: .bind <module> <key(s)>");
        return;
    }

    std::shared_ptr<Module> mod = ModuleManager::getModule(modName);

    if (mod != nullptr) {
        SettingType<std::string>* setting = mod->settings.getSettingByName<std::string>("keybind");
        if (setting != nullptr) {
            setting->value = keys;
            addCommandMessage("Successfully set <{}>'s keybind to <{}>", modName, keys);
        }
        else addCommandMessage("This module isn't keybindable. Use §c.bind list §rfor the list of keybindable modules");
    }
    else addCommandMessage("Invalid Module Name. Use §c.bind list §rfor the list of keybindable modules");

}
