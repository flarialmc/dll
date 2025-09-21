#include "PathCommand.hpp"

#include <Utils/WinrtUtils.hpp>

void PathCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) WinrtUtils::openSubFolder("Flarial");
    else {
        std::string arg1 = String::toLower(args[0]);
        if (arg1.contains("skin")) WinrtUtils::openSubFolder("Flarial\\Skins");
        else if (arg1.contains("config")) WinrtUtils::openSubFolder("Flarial\\Config");
        else if (arg1.contains("crosshair")) WinrtUtils::openSubFolder("Flarial\\Crosshairs");
        else if (arg1.contains("logs")) WinrtUtils::openSubFolder("Flarial\\logs");
        else if (arg1.contains("message") || arg1.contains("chat")) WinrtUtils::openSubFolder("Flarial\\MessageLogger");
        else if (arg1.contains("script")) WinrtUtils::openSubFolder("Flarial\\Scripts");
        else WinrtUtils::openSubFolder("Flarial");
    }
}