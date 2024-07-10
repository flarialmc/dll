#pragma once

#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/PlaySoundPacket.hpp"
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>
#include <chrono>
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/CommandRequestPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

class AutoRQListener : public Listener {
    Module *module;

    std::string gamemode = "";
    bool triggered = false;
    bool chs = false;

    void onPacketReceive(PacketEvent &event) override{
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::PlaySoundA) {
            auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());

            auto player = SDK::clientInstance->getLocalPlayer();
            if (pkt->mName == "raid.horn") {
                triggered = true;




                FlarialGUI::Notify("Re queuing into " + gamemode);




                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                command_packet->command = "/q " + gamemode;

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;
                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
            }
        }

        if (id == MinecraftPacketIds::ChangeDimension) {


            if(chs){

                std::thread ts([this]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    triggered = true;
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                    command_packet->command = "/connection";

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                    //notifs.push_back("Detecting gamemode " + gamemode);
                    chs = false;
                });
                ts.detach();
            }else{
                chs = true;

            }



        }
        else if (id == MinecraftPacketIds::Text) {
            auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";
            if (pkt->message.find(textToCheck) != std::string::npos && triggered) {
                std::string server = pkt->message.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                gamemode = std::regex_replace(server, pattern, "");
                triggered = false;
                event.cancel();

            }
            else if (pkt->message.find(textToCheckToSilence) != std::string::npos) {
                event.cancel();
            }
        }

    }

    void onPacketSend(PacketEvent &event) override {
        /*if (event.getPacket()->getId() == MinecraftPacketIds::Text) {
            TextPacket *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            std::cout << pkt->message << std::endl;
            
        }
        */
    }

public:
    explicit AutoRQListener(const char string[5], Module *

    module) {
        this->name = string;
        this->module = module;
    }
};
