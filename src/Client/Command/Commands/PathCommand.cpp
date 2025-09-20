#include "PathCommand.hpp"

#include <Utils/WinrtUtils.hpp>

void PathCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) WinrtUtils::openSubFolder("Flarial");
    else if (String::toLower(args[0]).contains("skin")) WinrtUtils::openSubFolder("Flarial\\Skins");
    else if (String::toLower(args[0]).contains("config")) WinrtUtils::openSubFolder("Flarial\\Config");
    else if (String::toLower(args[0]).contains("crosshair")) WinrtUtils::openSubFolder("Flarial\\Crosshairs");
    else if (String::toLower(args[0]).contains("logs")) WinrtUtils::openSubFolder("Flarial\\logs");
    else if (String::toLower(args[0]).contains("message") || String::toLower(args[0]).contains("chat")) WinrtUtils::openSubFolder("Flarial\\MessageLogger");
    else if (String::toLower(args[0]).contains("script")) WinrtUtils::openSubFolder("Flarial\\Scripts");
    else WinrtUtils::openSubFolder("Flarial");
}