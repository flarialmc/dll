#pragma once

#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/CommandRequestPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include <Windows.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

class CommandHotkeyListener : public Listener {

    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;

    Module *module;

    void onKey(KeyEvent &event) override {
            if (module->isEnabled()) {
                std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
                if (duration.count() >= 2.5) {
                    if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
                        std::shared_ptr<Packet> packet = SDK::createPacket(77);
                        auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                        command_packet->command = module->settings.getSettingByName<std::string>("command")->value;

                        command_packet->origin.type = CommandOriginType::Player;
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

