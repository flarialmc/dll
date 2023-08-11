#pragma once

#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK//Client/Network/Packet/TextPacket.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include <Windows.h>

class AutoGGListener : public Listener {

    Module* module;

    void onPacketReceive(PacketEvent& event) override {


        if (module->settings.getSettingByName<bool>("enabled")){


        bool innanillah = false;

            if (event.getPacket()->getId() == MinecraftPacketIds::SetTitle) {

                SetTitlePacket *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket().get());
                std::string amongus;

                std::string allahuakbar = Utils::removeNonAlphanumeric(Utils::removeColorCodes(pkt->text));

                if (allahuakbar.find("won") != std::string::npos || allahuakbar.find("lost") != std::string::npos || allahuakbar.find("spectator") != std::string::npos || allahuakbar.find("last") != std::string::npos || allahuakbar.find("Over") != std::string::npos) {

                    innanillah = true;

                }
            }

        if(innanillah) {

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

    void onPacketSend(PacketEvent& event) override {



    }


public:
    explicit AutoGGListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

