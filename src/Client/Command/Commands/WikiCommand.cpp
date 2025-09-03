#include "WikiCommand.hpp"
#include "../../Module/Manager.hpp"

#include <Utils/WinrtUtils.hpp>


void WikiCommand::execute(const std::vector<std::string> &args) {

    if (args.empty() || !(args[0] == "open" || args[0] == "copy")) {
        addCommandMessage("§cUsage: .wiki open/copy");
        return;
    }

    LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
    if (!player) {
        addCommandMessage("§cPlayer not available.");
        return;
    }

    Item* item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
                    SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot()
                 )->getItem();

    if (!item) {
        addCommandMessage("§cYou\'re not holding anything!");
        return;
    }

    std::string link = std::format("https://minecraft.wiki/w/{}", item->getname());

    if (args[0] == "open") {
        WinrtUtils::launchURI(link);
        addCommandMessage(std::format("Launched the wiki page for <{}>!", item->name));
        return;
    }
    else if (args[0] == "copy") {
        WinrtUtils::setClipboard(link);
        addCommandMessage(std::format("Copied the wiki link for <{}>!", item->name));
        return;
    }


}
