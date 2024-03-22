#pragma once

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

class CommandHotkeyListener : public Listener {

    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;

    Module *module;

    // Function to generate a UUID v4 as bytes
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

    static std::string convertUUIDBytesToString(const std::vector<uint8_t> &uuidBytes) {
        std::stringstream ss;

        for (const auto &byte: uuidBytes) {
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

    void onKey(KeyEvent &event) override {
            if (module->isEnabled()) {
                std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
                if (duration.count() >= 2.5) {
                    if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
                        std::shared_ptr<Packet> packet = SDK::createPacket(77);
                        auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                        //std::vector<uint8_t> request_id = UUIDtoMC();

                        command_packet->command = module->settings.getSettingByName<std::string>("command")->value;

                        command_packet->origin.type = CommandOriginType::Player;
                        //command_packet->origin.requestId = convertUUIDBytesToString(request_id);

                        command_packet->InternalSource = true;

                        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                        last_used = std::chrono::high_resolution_clock::now();
                    }
                }
            }
    }


public:
    explicit CommandHotkeyListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

