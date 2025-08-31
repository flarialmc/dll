#include "CommandManager.hpp"

#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "Commands/BindCommand.hpp"
#include "Commands/ConfigCommand.hpp"
#include "Commands/PrefixCommand.hpp"
#include "Commands/HelpCommand.hpp"
#include "Commands/LuaCommand.hpp"
#include "Commands/PathCommand.hpp"
#include "Commands/EjectCommand.hpp"
#include "Commands/TestCommand.hpp"
#include "Commands/SkinStealCommand.hpp"
#include "Commands/SpotifyCommand/SpotifyCommand.hpp"
#include "Commands/WikiCommand.hpp"
#include "Commands/IRCChat.hpp"
#include "../Client.hpp"

std::vector<std::shared_ptr<Command>> CommandManager::Commands = std::vector<std::shared_ptr<Command>>();
CommandManager CommandManager::instance;

void CommandManager::initialize() {
#if defined(__DEBUG__)
    Commands.push_back(std::make_unique<TestCommand>());
#endif
    Commands.push_back(std::make_unique<HelpCommand>());
    Commands.push_back(std::make_unique<PrefixCommand>());
    Commands.push_back(std::make_unique<LuaCommand>());
    Commands.push_back(std::make_unique<PathCommand>());
    Commands.push_back(std::make_unique<EjectCommand>());
    Commands.push_back(std::make_unique<SkinStealCommand>());
    Commands.push_back(std::make_unique<ConfigCommand>());
    Commands.push_back(std::make_unique<SpotifyCommand>());
    Commands.push_back(std::make_unique<BindCommand>());
    Commands.push_back(std::make_unique<WikiCommand>());
    //Commands.push_back(std::make_unique<IRCChat>());
Listen(&CommandManager::instance, PacketSendEvent, &CommandManager::onPacket);
}

void CommandManager::onPacket(PacketSendEvent &event) {
    MinecraftPacketIds id = event.getPacket()->getId();
    if (id != MinecraftPacketIds::Text) return;

    auto pkt = reinterpret_cast<TextPacket*>(event.getPacket());
    if (!pkt || pkt->message.empty() || pkt->message[0] != Client::settings.getSettingByName<std::string>("dotcmdprefix")->value[0]) return;

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
        SDK::clientInstance->getGuiData()->displayClientMessage("§cInvalid command. Use §b" + Client::settings.getSettingByName<std::string>("dotcmdprefix")->value + "help §cto see all available commands.");
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
    Commands.clear();
}
