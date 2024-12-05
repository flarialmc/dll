#pragma once

#include "../../Client/Events/Listener.hpp"
#include "../../Client/Events/Input/KeyEvent.hpp"
#include "../../Client/Events/Render/RenderEvent.hpp"
#include "../../Client/Events/EventManager.hpp"
#include "../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "CommandManager.hpp"

class CommandService : public Listener {
public:
    bool enabled = false;

    void onPacket(PacketSendEvent &event) {
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if(CommandManager::executeCommand(pkt->message)){
                event.setCancelled(true);
            }

        }
    }


    CommandService() {
        Listen(this, PacketSendEvent, &CommandService::onPacket);

        CommandManager::command("help", [](const std::vector<std::string>& args) {
            SDK::clientInstance->getGuiData()->displayClientMessage("Command list: ");
            for (const auto& command : CommandManager::commands){
                SDK::clientInstance->getGuiData()->displayClientMessage("  ." + command.first);
            }
        });
    }

    ~CommandService() {
        Deafen(this, PacketSendEvent, &CommandService::onPacket);
    }

};