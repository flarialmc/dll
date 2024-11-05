#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"


class AutoGG : public Module {

public:
    AutoGG() : Module("Auto GG", "Automatically sends a message when you\nwin a game. (Doesn't work everywhere)",
                      IDR_LIKE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, PacketEvent, &AutoGG::onPacketReceive)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, PacketEvent, &AutoGG::onPacketReceive)
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string) "GG");
    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addTextBox("Text", "The text to send.", settings.getSettingByName<std::string>("text")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onPacketReceive(PacketEvent &event) {
        bool triggered = false;
        MinecraftPacketIds id = event.getPacket()->getId();

        // TODO: add support for other servers (look for "won the game" text)
        if (id == MinecraftPacketIds::PlaySoundA) {
            auto *pkt = reinterpret_cast<PlaySoundPacket *>(event.getPacket());
            if (pkt->mName == "ui.toast.challenge_complete_java") {
                triggered = true;
            }


            if (triggered) {
                std::string win_message = this->settings.getSettingByName<std::string>("text")->value;
                if (!win_message.empty()) {
                    auto player = SDK::clientInstance->getLocalPlayer();
                    std::shared_ptr<Packet> packet = SDK::createPacket(9);
                    auto *text = reinterpret_cast<TextPacket *>(packet.get());


                    text->type = TextPacketType::CHAT;
                    text->message = win_message;
                    text->platformId = "";
                    text->translationNeeded = false;
                    text->xuid = "";
                    text->name = player->getPlayerName();

                    SDK::clientInstance->getPacketSender()->sendToServer(text);
                }
            }
        }

        else if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if (pkt->message == "§c§l» §r§c§lGame OVER!") {
                triggered = true;
            }


            if (triggered) {
                std::string win_message = this->settings.getSettingByName<std::string>("text")->value;
                if (!win_message.empty()) {
                    auto player = SDK::clientInstance->getLocalPlayer();
                    std::shared_ptr<Packet> packet = SDK::createPacket(9);
                    auto *text = reinterpret_cast<TextPacket *>(packet.get());


                    text->type = TextPacketType::CHAT;
                    text->message = win_message;
                    text->platformId = "";
                    text->translationNeeded = false;
                    text->xuid = "";
                    text->name = player->getPlayerName();

                    SDK::clientInstance->getPacketSender()->sendToServer(text);
                }
            }
        }
    }
};

