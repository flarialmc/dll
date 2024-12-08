#include "CommandManager.hpp"

#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "Commands/Help/Help.hpp"
#include "Commands/Prefix/Prefix.hpp"

std::vector<std::unique_ptr<Command>> CommandManager::Commands = std::vector<std::unique_ptr<Command>>();
CommandManager CommandManager::instance;

void CommandManager::initialize() {
    Commands.push_back(std::make_unique<Prefix>());
    Commands.push_back(std::make_unique<Help>());
    Listen(&CommandManager::instance, PacketSendEvent, &CommandManager::onPacket);
}

void CommandManager::onPacket(PacketSendEvent& event) {
    MinecraftPacketIds id = event.getPacket()->getId();
    if (id != MinecraftPacketIds::Text) return;

    auto pkt = reinterpret_cast<TextPacket*>(event.getPacket());
    if (!pkt || pkt->message.empty() || pkt->message[0] != Client::settings.getSettingByName<std::string>("prefix")->value[0]) return;

    event.setCancelled(true);

    // Remove the prefix
    std::string cmd = pkt->message.substr(1);
    const std::vector<std::string> args = String::split(cmd, ' ');

    if (args.empty()) return;

    const std::string_view commandName = args[0];

    // Find the command that matches the commandName
    auto it = std::ranges::find_if(Commands, [&](const auto& command) {
        if (command->Name == commandName)
            return true;

        // Check if any alias matches the commandName
        return std::ranges::any_of(command->Aliases, [&](const std::string& alias) {
            return alias == commandName;
            });
        });

    if (it == Commands.end()) {
        SDK::clientInstance->getGuiData()->displayClientMessage("§cInvalid command. Use §b.help §cto see all available commands.");
        return;
    }

    // Exclude command name from args
    // Example: .prefix *
    //                  ^ this becomes args[0] instead of args[1]
    std::vector commandArgs(args.begin() + 1, args.end());
    (*it)->execute(commandArgs);
}

void CommandManager::terminate() {
    Deafen(&CommandManager::instance, PacketSendEvent, &CommandManager::onPacket);
}