#include "Help.hpp"

#include "../../CommandManager.hpp"

void Help::execute(const std::vector<std::string>& args) {
    SDK::clientInstance->getGuiData()->displayClientMessage("§c§lCommands: ");
    for (const auto& command : CommandManager::Commands) {
        //SDK::clientInstance->getGuiData()->displayClientMessage("§r§l§9" + command->Name + " §3§l»§r " + command->Description);
        SDK::clientInstance->getGuiData()->displayClientMessage(fmt::format("  §l§9{}{} §3§l»§r {}", Client::settings.getSettingByName<std::string>("prefix")->value, command->Name, command->Description));
    }
}