#include "PathCommand.hpp"

#include <Utils/WinrtUtils.hpp>

#include "Utils/PlatformUtils.hpp"

void PathCommand::open(std::string folder) {
    bool created = false;

    if (PlatformUtils::isUWP()) {
        std::filesystem::path folder_path(Utils::getRoamingPath() + "\\Flarial\\" + folder);
        if (!exists(folder_path)) {
            create_directory(folder_path);
            created = true;
        }
        WinrtUtils::openSubFolder("Flarial\\" + folder);
    }
    else {
        std::filesystem::path folder_path(PlatformUtils::getRoamingPath() + "\\Flarial\\Client\\" + folder);
        if (!exists(folder_path)) {
            create_directory(folder_path);
            created = true;
        }
        PlatformUtils::openFolder(PlatformUtils::getRoamingPath() + "\\Flarial\\Client\\" + folder);
    }

    if (created) addCommandMessage(std::format("Created and opened the <{}> folder!", folder));
    else addCommandMessage(std::format("Opened the <{}> folder!", folder));
}

void PathCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) open();
    else {
        std::string arg1 = String::toLower(args[0]);
        if (arg1.contains("skin")) open("Skins");
        else if (arg1.contains("config")) open("Config");
        else if (arg1.contains("crosshair")) open("Crosshairs");
        else if (arg1.contains("logs")) open("logs");
        else if (arg1.contains("message") || arg1.contains("chat")) open("MessageLogger");
        else if (arg1.contains("script")) open("Scripts");
        else if (arg1.contains("geo")) open("Geometry");
        else open();
    }
}
