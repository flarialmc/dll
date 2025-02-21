#include "TestCommand.hpp"

#include <Utils/WinrtUtils.hpp>
#include <Scripting/Scripting.hpp>

void TestCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .test <uri/pick/notification/box/clipboard>");
        return;
    }

    const std::string action = String::toLower(args[0]);

    if (action == "uri") {
        WinrtUtils::launchURI("https://flarial.xyz");
    }
    if (action == "pick") {
        WinrtUtils::pickFiles();
    }
    if (action == "copy") {
        WinrtUtils::pickAndCopyFiles(L"*", "\\assets");
    }
    if (action == "notification") {
        WinrtUtils::showNotification("Flarial", "Hi");
    }
    if (action == "box") {
        WinrtUtils::showMessageBox("Flarial", "Your Minecraft version is not supported.");
    }
    if (action == "clipboard") {
        if (args[1].empty()) return;

        if (args[1] == "get") {
            addCommandMessage(WinrtUtils::getClipboard());
        }
        if (args[1] == "set") {
            if (args[2].empty()) return;
            WinrtUtils::setClipboard(args[2]);
        }
    }



}
