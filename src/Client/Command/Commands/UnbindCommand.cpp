#include "UnbindCommand.hpp"
#include "../../Module/Manager.hpp"
#include <regex>
#include "Client.hpp"

void UnbindCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .unbind list / .unbind <module>");
        return;
    }

    if (args[0] == "list") {
        std::vector<std::string> modList = {
            "Zoom",
            "Player_Notifier",
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

    if (String::toLower(args[0]) == "eject") {
        Client::settings.getSettingByName<std::string>("ejectKeybind")->value = "";
        Client::SavePrivate();
        Client::LoadPrivate();
        addCommandMessage("Successfully unbinded <Eject>.");
        return;
    }
    else if (String::toLower(args[0]) == "clickgui") {
        addCommandMessage("ClickGUI isn\'t unbindable.");
        return;
    }
    else {
        for (auto pair : ModuleManager::moduleMap) {
            if (String::toLower(pair.second->name) == String::toLower(args[0])) {
                SettingType<std::string>* setting = pair.second->settings.getSettingByName<std::string>("keybind");
                if (setting != nullptr) {
                    setting->value = "";
                    Client::SaveSettings();
                    addCommandMessage("Successfully unbinded <{}>.", pair.second->name);
                }
                else addCommandMessage("This module isn't keybindable. Use §c.unbind list §rfor the list of keybindable modules");
                return;
            }
        }
    }

    addCommandMessage("Invalid Module Name. Use §c.unbind list §rfor the list of keybindable modules");

}
