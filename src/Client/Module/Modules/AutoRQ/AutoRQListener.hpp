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
#include <string>
#include <sstream>
#include <random>
#include <algorithm> 
#include <cctype>
#include <locale>

class AutoRQListener : public Listener {
    Module *module;

    std::string currentGame;
    bool triggered = false;

    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    }

    void onPacketReceive(PacketEvent &event) override {
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::SetTitle) {
            if(module->settings.getSettingByName<bool>("solo")->value){
                auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());

                if (pkt->text == "§cYou're a spectator!" ||
                    pkt->text == "§cYou died!" ||
                    pkt->text == "§7You're spectating the §as§eh§6o§cw§7!") {
                    triggered = true;
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

                    command_packet->command = "/connection";

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                } //std::cout << pkt->mName << std::endl;
            }
        }
        if (id == MinecraftPacketIds::PlaySoundA) {
            if(module->settings.getSettingByName<bool>("solo")->value){
                auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());

                if (pkt->mName == "hive.grav.game.portal.reached.final") {
                    triggered = true;
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

                    command_packet->command = "/connection";

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                } //std::cout << pkt->mName << std::endl;
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("AutoMapAvoider")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
                std::string maps_to_avoid = module->settings.getSettingByName<std::string>("text")->value;
                std::stringstream ss(maps_to_avoid);
                std::string fixed_value ="§b§l» §r§e";
                std::vector<int> vect;
                std::vector<std::string> result;
                if (pkt->message.substr(0,15)==fixed_value){
                    while( ss.good() )
                    {
                        std::string substr;
                        getline( ss, substr, ',' );
                        ltrim(substr);
                        result.push_back( substr );
                    }

                    for (std::size_t i = 0; i < result.size(); i++) {
                        std::string evaluate_string = fixed_value + result[i];
                        std::transform(evaluate_string.begin(), evaluate_string.end(), evaluate_string.begin(), ::tolower);
                        std::transform(pkt->message.begin(), pkt->message.end(), pkt->message.begin(), ::tolower);
                        if (pkt->message.substr(0, (evaluate_string.length())) == evaluate_string)  { 
                            triggered = true;
                            FlarialGUI::Notify("Found map: " + result[i]);
                            std::shared_ptr<Packet> packet = SDK::createPacket(77);
                            auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                            command_packet->command = "/connection";
                            command_packet->origin.type = CommandOriginType::Player;
                            command_packet->InternalSource = true;
                            SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                        }
                    }
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            //if(!module->settings.getSettingByName<bool>("solo")->value) {
                if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                    triggered = true;
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                    command_packet->command = "/connection";

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                    return;
                } //std::cout << pkt->mName << std::endl;
            //}

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";

            if (pkt->message.find(textToCheck) != std::string::npos && triggered) {
                std::string server = pkt->message.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                std::string name = std::regex_replace(server, pattern, "");
                FlarialGUI::Notify("Preparing Q: " + name);
                std::thread t([name]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                    FlarialGUI::Notify("Executing command /q " + name);

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                    command_packet->command = "/q " + name;

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                });
                t.detach();
                triggered = false;
                pkt->message = "";

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
