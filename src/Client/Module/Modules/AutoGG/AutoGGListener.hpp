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



    }
    void onPacketSend(PacketEvent& event) override {



    }


public:
    explicit AutoGGListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

