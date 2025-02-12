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
        WinrtUtils::launchURI("https://pornhub.com");
    }
    if (action == "pick") {
        WinrtUtils::pickFiles();
    }
    if (action == "notification") {
        WinrtUtils::showNotification("Nigger", "Hi");
    }
    if (action == "box") {
        WinrtUtils::showMessageBox("Nigger", "Hi your version isnt supported :(");
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
