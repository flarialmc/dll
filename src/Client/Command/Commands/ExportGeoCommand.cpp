#include "ExportGeoCommand.hpp"

#include <fstream>
#include <filesystem>

#include "Utils/Utils.hpp"
#include "Utils/Logger/Logger.hpp"
#include "SDK/Client/Util/json.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"

void ExportGeoCommand::execute(const std::vector<std::string>& args) {
    auto* player = SDK::clientInstance->getLocalPlayer();
    if (!player) {
        addCommandMessage("§cNot in a world.");
        return;
    }

    auto* level = player->getLevel();
    if (!level) return;

    // Determine target player name (default: self)
    std::string targetName;
    if (args.empty()) {
        targetName = String::removeNonAlphanumeric(
            String::removeColorCodes(String::toLower(*player->getNametag())));
    } else {
        targetName = String::toLower(args[0]);
    }

    // Search the player map for the target
    bool found = false;
    for (const auto& [uuid, entry] : player->getLevel()->getPlayerMap()) {
        std::string entryName = String::removeNonAlphanumeric(
            String::removeColorCodes(String::toLower(entry.name)));

        if (entryName != targetName) continue;
        found = true;

        // Read geometry from MinecraftJson::Value
        MinecraftJson::Value geoSnap;
        memcpy(&geoSnap, &entry.playerSkin.mGeometryData, sizeof(MinecraftJson::Value));
        nlohmann::json geo = geoSnap.toNlohmannJson();

        if (geo.is_null()) {
            addCommandMessage("§c{} has no geometry data.", entry.name);
            break;
        }

        // Ensure output directory exists
        std::filesystem::path folder(Utils::getRoamingPath() + "\\Flarial\\Geometry");
        if (!exists(folder)) create_directories(folder);

        std::string filename = String::removeNonAlphanumeric(entry.name) + "_geometry.json";
        std::filesystem::path filePath = folder / filename;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            addCommandMessage("§cFailed to open file for writing.");
            break;
        }

        file << geo.dump(2);
        file.close();

        addCommandMessage("§aExported {}'s geometry to §b{}",
                          entry.name, filename);
        addCommandMessage("§7Type §b.path geometry §7to open the folder.");
        Logger::info("[ExportGeo] saved geometry for {} to {}", entry.name, filePath.string());
        break;
    }

    if (!found) {
        addCommandMessage("§cPlayer {} not found.", targetName);
    }
}
