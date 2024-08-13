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

    std::string gamemode = "";
    bool connection_triggered = false;
    bool chs = false;
    bool queue_triggered = false;

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
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::PlaySoundA) {
            if(module->settings.getSettingByName<bool>("solo")->value){
                auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());

                if (pkt->mName == "hive.grav.game.portal.reached.final") {
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("murderer")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§c§l» §r§c§lMurderer") {
                    FlarialGUI::Notify("Found role Murderer");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("sheriff")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§9§l» §r§9§lSheriff") {
                    FlarialGUI::Notify("Found role Sheriff");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("innocent")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§a§l» §r§a§lInnocent") {
                    FlarialGUI::Notify("Found role Innocent");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("death")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§d§l» §r§bYou are a §cDeath") {
                    FlarialGUI::Notify("Found role Death");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("runner")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§d§l» §r§bYou are a §aRunner") {
                    FlarialGUI::Notify("Found role Runner");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("hider")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§e§l» §rYou are a §eHIDER") {
                    FlarialGUI::Notify("Found role Hider");
                    queue_triggered = true;
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            if(module->settings.getSettingByName<bool>("seeker")->value){
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

                if (pkt->message == "§c§l» §rYou are a §cSEEKER") {
                    FlarialGUI::Notify("Found role Seeker");
                    queue_triggered = true;
                }
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
                            FlarialGUI::Notify("Found map: " + result[i]);
                            queue_triggered = true;
                        }
                    }
                }
            }
        }
        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            //if(!module->settings.getSettingByName<bool>("solo")->value) {
                if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                    queue_triggered = true;
                }
            //}

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";

            if (pkt->message.find(textToCheck) != std::string::npos && connection_triggered) {
                std::string server = pkt->message.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                gamemode = std::regex_replace(server, pattern, "");
                connection_triggered = false;
                //FlarialGUI::Notify("Detecting gamemode " + gamemode);
                event.cancel();
            }
            else if (pkt->message.find(textToCheckToSilence) != std::string::npos) {
                event.cancel();
            }
        }
        if (id == MinecraftPacketIds::ChangeDimension) {
            if(chs){
                std::thread ts([this]() {
                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                    connection_triggered = true;

                    command_packet->command = "/connection";

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;

                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                    chs = false;
                });
                ts.detach();
            }else{
                chs = true;

            }
        }
        if (queue_triggered == true){
            if(module->settings.getSettingByName<bool>("hub")->value){
            queue_triggered = false;
            std::shared_ptr<Packet> packet = SDK::createPacket(77);
            auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

            FlarialGUI::Notify("Executing command /hub");

            command_packet->command = "/hub";

            command_packet->origin.type = CommandOriginType::Player;

            command_packet->InternalSource = true;

            SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
            }
            else{
            queue_triggered = false;
            std::shared_ptr<Packet> packet = SDK::createPacket(77);
            auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

            FlarialGUI::Notify("Re queuing into " + gamemode);

            command_packet->command = "/q " + gamemode;

            command_packet->origin.type = CommandOriginType::Player;

            command_packet->InternalSource = true;

            SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
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
