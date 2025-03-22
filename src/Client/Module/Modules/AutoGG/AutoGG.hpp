#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/PlaySoundPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"


class AutoGG : public Module {

public:
    AutoGG() : Module("Auto GG", "Automatically sends a message when you win a game. Workes on\nThe Hive, Zeqa, CubeCraft, Lifeboat, Galaxite and Mineville. ",
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

    void defaultConfig() override { Module::defaultConfig();
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
        MinecraftPacketIds id = event.getPacket()->getId();

        // TODO: add support for other servers (look for "won the game" text)
        if (id == MinecraftPacketIds::SetTitle) {
            auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());
            static const std::regex rgxChRu(R"(Is The §6§l(Chronos|Rush) (Champion|Champions)!)");
            if (
                //Zeqa
                pkt->text == "§f§aYou won the game!" || //pre s8
                pkt->text == "§f§cYou lost the game!" || //pre s8
                pkt->text == "   " || //s8
                pkt->text == "   " || //s8
                //Galaxite CREDIT @1unar-Eclipse
                pkt->text.find("Team§r§a won the game!")!= std::string::npos ||
                pkt->text.find("§bHiders§r§f Win")!= std::string::npos ||
                pkt->text.find("§eSeekers§r§f Win")!= std::string::npos ||
                pkt->text == "Finished" ||
                pkt->text == "Out of Time!" ||
                std::regex_search(pkt->text, rgxChRu) ||
                //Mineville
                pkt->text == "§aYou Win!" ||
                pkt->text == "§cGame Over!") {

                SendGG();
            }
        }

        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());
            if (pkt->message == "§c§l» §r§c§lGame OVER!" || //The Hive
                pkt->message.find("§r§a won the game!")!= std::string::npos || //CubeCraft
                pkt->message.find("§a has won the game!")!= std::string::npos){ //Lifeboat
                SendGG();
            }
        }
    }
    void SendGG(){
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
};

