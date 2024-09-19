#pragma once

#include <regex>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Network/PacketEvent.hpp"
#include "../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"

class HiveModeCatcherListener : public Listener {
    bool connectionExecuted = false;
    bool listenForServer = false;




    void onPacketReceive(PacketEvent &event) override {

        if (SDK::getServerIP().find("hive") == std::string::npos) return;
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::ChangeDimension) {
            if (connectionExecuted) {
                listenForServer = true;
                connectionExecuted = false;
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                command_packet->command = "/connection";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;


                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
            } else {
                connectionExecuted = true;
            }
        }

        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";


            if (pkt->message.find(textToCheck) != std::string::npos && listenForServer) {
                std::string server = pkt->message.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                HiveModeCatcherListener::currentGame = std::regex_replace(server, pattern, "");
                event.cancel();
                listenForServer = true;

            } else if (pkt->message.find(textToCheckToSilence) != std::string::npos) {
                event.cancel();
            }

        }
    }

public:
    explicit HiveModeCatcherListener(const char string[5]) {
        this->name = string;
    }
    static inline std::string currentGame;
};
