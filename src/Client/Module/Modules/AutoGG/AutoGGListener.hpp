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

class AutoGGListener : public Listener {
    Module *module;


    void onPacketReceive(PacketEvent &event) override {
        bool triggered = false;
        MinecraftPacketIds id = event.getPacket()->getId();

        // TODO: add support for other servers (look for "won the game" text)
        if (id == MinecraftPacketIds::PlaySoundA) {
            auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());
            if (pkt->mName == "ui.toast.challenge_complete_java" || pkt->mName == "raid.horn") {
                triggered = true;
            }


            if (triggered) {
                std::string win_message = module->settings.getSettingByName<std::string>("text")->value;
                if (!win_message.empty()) {
                    auto player = SDK::clientInstance->getLocalPlayer();
                    std::shared_ptr<Packet> packet = SDK::createPacket(9);
                    auto *text = reinterpret_cast<TextPacket *>(packet.get());


                    text->type = TextPacketType::CHAT;
                    text->message = win_message;
                    text->platformId = "";
                    text->translationNeeded = false;
                    text->xuid = "";
                    text->name = player->playerName;

                    SDK::clientInstance->getPacketSender()->sendToServer(text);
                }
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
    explicit AutoGGListener(const char string[5], Module *

                            module) {
        this->name = string;
        this->module = module;
    }
};
