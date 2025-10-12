#include "WaypointCommand.hpp"

#include "../../Module/Manager.hpp"
#include "../../Module/Modules/Waypoints/Waypoints.hpp"

bool WaypointCommand::checkValid(const std::string& s) {
    if (s.empty()) return false;
    if (s == "~") return true;

    bool decimal = false;
    for (const char i : s) {
        if (i == '.') {
            if (decimal) return false;
            decimal = true;
        }
        else if (!std::isdigit(i)) return false;
    }
    return true;
}

void WaypointCommand::execute(const std::vector<std::string> &args) {

    if (args.empty() || args[0] == "help") {
        addCommandMessage(
            "§cAliases:§r waypoints, wp\n"
            "                  §cUsage:§r\n"
            "                  .waypoint add ~x ~y ~z\n"
            "                  .waypoint add ~x ~z  (y = 63)\n"
            "                  .waypoint delete/remove <name>\n"
            "                  .waypoint ~x ~z  (y = 63)\n"
            "                  .waypoint ~x ~y z/~\n"
        );
        return;
    }

    std::shared_ptr<Waypoints> waypoints = std::dynamic_pointer_cast<Waypoints>(ModuleManager::getModule("Waypoints"));

    if (args[0] == "delete" || args[0] == "remove") {
        if (args.size() >= 2) {
            if (args[1].empty()) {
                addCommandMessage("§cInvalid waypoint name§r. Use quotes for names with spaces.");
                return;
            }

            std::string wpName = args[1];
            if (args[1][0] == '"') {
                for (int i = 2; i < args.size(); i++) {
                    wpName += " " + args[i];
                }
                wpName = wpName.substr(1, wpName.size() - 2);
            }

            bool deleted = waypoints->deleteWaypoint(wpName);
            if (!deleted) addCommandMessage("Waypoint <{}> not found.", wpName);
            else addCommandMessage("Deleted waypoint <{}>!", wpName);

            return;
        }
        addCommandMessage("§cInvalid waypoint name§r. Use quotes for names with spaces.");
        return;
    }

    int offset = 0;

    if (args[0] == "add") offset = 1;
    else if (!checkValid(args[0])) {
        addCommandMessage("§cInvalid coordinate(s)§r: use a number or \'~\' for the current position. Use .waypoint help for more info.");
        return;
    }

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    if (args.size() == 2 + offset) {
        if (!checkValid(args[offset]) || !checkValid(args[1 + offset])) {
            addCommandMessage("§cInvalid coordinate(s)§r: use a number or \'~\' for the current position. Use .waypoint help for more info.");
            return;
        }

        x = args[offset] == "~" ? SDK::clientInstance->getLocalPlayer()->getPosition()->x : std::stof(args[1]);
        y = 63.f;
        z = args[1 + offset] == "~" ? SDK::clientInstance->getLocalPlayer()->getPosition()->z : std::stof(args[2]);
    }
    else if (args.size() == 3 + offset) {
        if (!checkValid(args[offset]) || !checkValid(args[1 + offset]) || !checkValid(args[2 + offset])) {
            addCommandMessage("§cInvalid coordinate(s)§r: use a number or \'~\' for the current position. Use .waypoint help for more info.");
            return;
        }

        x = args[offset] == "~" ? SDK::clientInstance->getLocalPlayer()->getPosition()->x : std::stof(args[offset]);
        y = args[1 + offset] == "~" ? SDK::clientInstance->getLocalPlayer()->getPosition()->y : std::stof(args[1 + offset]);
        z = args[2 + offset] == "~" ? SDK::clientInstance->getLocalPlayer()->getPosition()->z : std::stof(args[2 + offset]);
    }
    else {
        addCommandMessage("§cInvalid amount of arguments§r. Use .waypoint help for more info.");
        return;
    }

    if (waypoints != nullptr) {
        int index = waypoints->getNextAvailableIndex();
        std::string col = "FFFFFF";
        if (waypoints->settings.getSettingByName<bool>("randomizeColor") != nullptr) {
            if (waypoints->getOps<bool>("randomizeColor")) col = FlarialGUI::ColorFToHex(D2D1_COLOR_F(waypoints->random(), waypoints->random(), waypoints->random(), 1.f));
        }
        waypoints->addWaypoint(
            index,
            "waypoint-" + FlarialGUI::cached_to_string(index),
            col,
            Vec3{ x, y, z },
            true,
            true,
            false,
            100.0f
        );
        addCommandMessage("Added waypoint at <{:.2f}, {:.2f}, {:.2f}>!", x, y, z);
        FlarialGUI::Notify("Added waypoint!");
    }
}
