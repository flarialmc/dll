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
        if (module->settings.getSettingByName<bool>("enabled")){

        TextPacket *pkt = reinterpret_cast<TextPacket *>(event.getPacket().get());
        std::string amongus;

        if (*pkt->getName(&amongus) == (std::string)"TextPacket") {
            std::string allahuakbar = Utils::removeNonAlphanumeric(Utils::removeColorCodes(pkt->message));
            if (allahuakbar == (std::string)"Queued Use the bed to return to lobby" or allahuakbar == (std::string)"Game OVER") {
                auto player = SDK::clientInstance->getLocalPlayer();
                std::string xuid = *player->getXuid(&xuid);
                std::shared_ptr<Packet> packet = SDK::createPacket(9);
                TextPacket* akbar = reinterpret_cast<TextPacket*>(packet.get());

                akbar->type = TextPacketType::CHAT;
                akbar->message = module->settings.getSettingByName<std::string>("text")->value;
                akbar->platformId = "";
                akbar->translationNeeded = false;
                akbar->xuid = xuid;
                akbar->name = player->playerName;

                SDK::clientInstance->getPacketSender()->sendToServer(akbar);

            }
        }
        }
    }

    void onPacketSend(PacketEvent& event) override {



    }


public:
    explicit AutoGGListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

