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

    std::string currentGame = "";
    bool triggered = false;

    void onPacketReceive(PacketEvent &event) override {
        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::SetTitle) {
            if(!module->settings.getSettingByName<bool>("solo")->value) return;
            auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());

            if (pkt->text == "§6§l»§r§c Game Over §6§l«" ||
                pkt->text == "§6§l»§r§c Game Over §6§l«") {
                triggered = true;
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

                command_packet->command = "/connection";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;

                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

            } //std::cout << pkt->mName << std::endl;

        }

        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if(!module->settings.getSettingByName<bool>("solo")->value) {
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
            }

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
