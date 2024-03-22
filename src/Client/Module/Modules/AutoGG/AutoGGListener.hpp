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
            if (pkt->mName == "ui.toast.challenge_complete_java" ||
                pkt->mName == "ui.toast.challenge_complete_java") {
                triggered = true;
            } //std::cout << pkt->mName << std::endl;


            if (triggered) {
                std::string win_message = module->settings.getSettingByName<std::string>("text")->value;
                if (!win_message.empty()) {
                    auto player = SDK::clientInstance->getLocalPlayer();
                    std::string xuid = *player->getXuid(&xuid);
                    std::shared_ptr<Packet> packet = SDK::createPacket(9);
                    auto *text = reinterpret_cast<TextPacket *>(packet.get());


                    text->type = TextPacketType::CHAT;
                    text->message = win_message;
                    text->platformId = "";
                    text->translationNeeded = false;
                    text->xuid = xuid;
                    text->name = player->playerName;

                    SDK::clientInstance->getPacketSender()->sendToServer(text);

                    /* Vec3 position = *player->getPosition();
                    std::shared_ptr<Packet> sound_packet = SDK::createPacket((int)MinecraftPacketIds::PlaySoundA);
                    PlaySoundPacket *sound = reinterpret_cast<PlaySoundPacket*>(sound_packet.get());
                    sound->mName = "step.sand";//"ui.toast.challenge_complete_java";
                    sound->mPos = Vec3<int>{ int(position.x), int(position.y), int(position.z) };
                    //sound->mPos = Vec3<int>{ 0, 66, 0 };
                    sound->mVolume = 0.8;
                    sound->mPitch = 1.6;

                    SDK::clientInstance->getPacketSender()->send(sound);*/
                    //SDK::clientInstance->getPacketSender()->send(text);
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
