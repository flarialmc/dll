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


    static std::vector<uint8_t> generateUUIDBytes() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        std::vector<uint8_t> uuidBytes(16);
        for (int i = 0; i < 16; ++i) {
            uuidBytes[i] = static_cast<uint8_t>(dis(gen));
        }

        // Set the version to 4
        uuidBytes[6] = (uuidBytes[6] & 0x0F) | 0x40;

        // Set the variant to RFC 4122
        uuidBytes[8] = (uuidBytes[8] & 0x3F) | 0x80;

        return uuidBytes;
    }

    static std::string convertUUIDBytesToString(const std::vector<uint8_t>& uuidBytes) {
        std::stringstream ss;

        for (const auto& byte : uuidBytes) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }

        return ss.str();
    }

    // Function to modify and return the UUID bytes
    static std::vector<uint8_t> UUIDtoMC() {
        // Generate a UUID v4
        std::vector<uint8_t> uuidBytes = generateUUIDBytes();

        // Reverse the first 8 bytes
        for (int i = 0; i < 4; ++i) {
            std::swap(uuidBytes[i], uuidBytes[7 - i]);
        }

        // Reverse the next 8 bytes
        for (int i = 8; i < 12; ++i) {
            std::swap(uuidBytes[i], uuidBytes[15 - i]);
        }

        return uuidBytes;
    }


    void onPacketReceive(PacketEvent &event) override {
        MinecraftPacketIds id = event.getPacket()->getId();

        // TODO: add support for other servers (look for "won the game" text)
        if (id == MinecraftPacketIds::PlaySoundA) {
            auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());

            if (pkt->mName == "raid.horn" ||
                pkt->mName == "raid.horn") {
                triggered = true;
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

                command_packet->command = "/connection";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;

                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                
            } //std::cout << pkt->mName << std::endl;

        }
        else if (id == MinecraftPacketIds::Text) {
            auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());
            //if (pkt->type == TextPacketType::SYSTEM) {
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
                    pkt->message = "";
                }
            //}
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
