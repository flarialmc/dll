#include "BindCommand.hpp"
#include "../../Module/Manager.hpp"
#include <regex>
#include "Client.hpp"

void BindCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .bind list / .bind <key(s)> / .bind <module> <key(s)>");
        return;
    }

    if (args[0] == "list") {
        std::vector<std::string> modList = {
            "Zoom",
            "Player_Notifier",
            "ClickGUI",
            "Toggle_Sprint",
            "Toggle_Sneak",
            "Java_Debug_Menu",
            "FreeLook",
            "Hive_Statistics",
            "Waypoints",
            "SnapLook",
            "Skin_Stealer",
            "Tab_List",
            "Eject"
        };
        std::string modListText = "Keybindable Module List: (put _ in place of spaces)";
        for (int i = 0; i < modList.size(); i++) modListText += std::format("\n{}. {}", i + 1, modList[i]);
        addCommandMessage(modListText);
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

    if (String::toLower(modName) == "eject") {
        Client::settings.getSettingByName<std::string>("ejectKeybind")->value = keys;
        Client::SavePrivate();
        Client::LoadPrivate();
        addCommandMessage("Successfully set <Eject> keybind to {}", keys);
        return;
    }
    else {
        for (auto pair : ModuleManager::moduleMap) {
            if (String::toLower(pair.second->name) == String::toLower(modName)) {
                SettingType<std::string>* setting = pair.second->settings.getSettingByName<std::string>("keybind");
                if (setting != nullptr) {
                    setting->value = keys;
                    Client::SaveSettings();
                    addCommandMessage("Successfully set <{}>'s keybind to <{}>", pair.second->name, keys);
                }
                else addCommandMessage("This module isn't keybindable. Use §c.bind list §rfor the list of keybindable modules");
                return;
            }
        }
    }

    addCommandMessage("Invalid Module Name. Use §c.bind list §rfor the list of keybindable modules");

}
