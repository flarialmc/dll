#pragma once

#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class AutoGGListener : public Listener {

    Module* module;

    void onPacketReceive(PacketEvent& event) override {
        TextPacket* pkt = reinterpret_cast<TextPacket*>(event.getPacket().get());
        std::string amongus;
        if (*pkt->getName(&amongus) == (std::string)"TextPacket") {
            TextPacket* akbar = reinterpret_cast<TextPacket*>(SDK::createPacket(0x9).get());
            akbar->setmessage((std::string)"Hello");

            SDK::clientInstance->getPacketSender()->sendToServer(reinterpret_cast<Packet*>(akbar));
        }

    }

public:
    explicit AutoGGListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

