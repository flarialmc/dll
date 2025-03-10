#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"


class ZeqaUtils : public Module {
private:
    std::string gamemode;
    bool ranked = false;
public:
    ZeqaUtils() : Module("Zeqa Utils", "Handy utilities for Zeqa",
                      IDR_TIME_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, PacketEvent, &ZeqaUtils::onPacketReceive)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, PacketEvent, &ZeqaUtils::onPacketReceive)
        Module::onDisable();
    }

    void defaultConfig() override {
        // if (settings.getSettingByName<bool>("req") == nullptr)
        //     settings.addSetting("req", false);
        if (settings.getSettingByName<bool>("promomessage") == nullptr)
            settings.addSetting("promomessage", false);
        if (settings.getSettingByName<bool>("join") == nullptr)
            settings.addSetting("join", false);
        if (settings.getSettingByName<bool>("leave") == nullptr)
            settings.addSetting("leave", false);
        if (settings.getSettingByName<bool>("killstreak") == nullptr)
            settings.addSetting("killstreak", false);

        if (settings.getSettingByName<bool>("friendaccept") == nullptr)
            settings.addSetting("friendaccept", false);
        if (settings.getSettingByName<bool>("partyaccept") == nullptr)
            settings.addSetting("partyaccept", false);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        // VVVV duels are broken at the moment so not done yet VVVV

        // this->addHeader("Auto re queue");
        // this->addToggle("Auto re queue", "automatically join the queue for another match once the current ends", this->settings.getSettingByName<bool>("req")->value);

        this->addHeader("Debloat chat");
        this->addToggle("Promo message", "Removes all promo/info messages", this->settings.getSettingByName<bool>("promomessage")->value);
        this->addToggle("player join", "Removes player join message", this->settings.getSettingByName<bool>("join")->value);
        this->addToggle("player leave", "Removes player leave message", this->settings.getSettingByName<bool>("leave")->value);
        this->addToggle("Killstreak", "Removes Message indicating a player has gotten a killstreak", this->settings.getSettingByName<bool>("killstreak")->value);

        this->addHeader("Auto accept");
        this->addToggle("Friend request", "Automatically accept incoming friend requests.", this->settings.getSettingByName<bool>("friendaccept")->value);
        this->addToggle("Party request", "Automatically accept incoming party requests.", this->settings.getSettingByName<bool>("partyaccept")->value);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }



    void onPacketReceive(PacketEvent &event) {
        MinecraftPacketIds id = event.getPacket()->getId();
        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

            if (this->settings.getSettingByName<bool>("req")->value){

            }

            if (this->settings.getSettingByName<bool>("promomessage")->value) {
                if (pkt->message == " " || pkt->message == " " ||
                    pkt->message == " " ||
                    pkt->message == " "){
                    FlarialGUI::Notify("promomessage");
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("join")->value) {

                if (pkt->message.substr(0,15) == "§8[§a+§8]§a") {
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("leave")->value) {

                if (pkt->message.substr(0,15) == "§8[§c-§8]§c") {
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("killstreak")->value) {
                if (pkt->message.contains("§g has gotten a ") && pkt->message.contains("§g killstreak")){
                    event.cancel();
                }
            }
            if (this->settings.getSettingByName<bool>("friendaccept")->value) {
                if(pkt->message.find("§l§q» §r§aYou have received a friend request from ") !=std::string::npos){

                    // std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    // auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                    // command_packet->command = "/f accept " + pkt->message.substr(40,  pkt->message.length()-44);
                    //
                    // command_packet->origin.type = CommandOriginType::Player;
                    //
                    // command_packet->InternalSource = true;
                    // SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                    //
                    FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(55, pkt->message.length() - 5));  //untested
                }
            }
            if (this->settings.getSettingByName<bool>("partyaccept")->value) {
                if (pkt->message.find("§l§8» §r§7Received a new party invite from ")!= std::string::npos){
                //
                //     std::shared_ptr<Packet> packet = SDK::createPacket(77);
                //     auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                //     command_packet->command = "/p accept " + pkt->message.substr(6, pkt->message.length()-40);
                //
                //     command_packet->origin.type = CommandOriginType::Player;
                //
                //     command_packet->InternalSource = true;
                //     SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
                //
                FlarialGUI::Notify("Accepted party invite from: " + pkt->message.substr(48, pkt->message.length())); //untested
                }
            }
        }
    }
};

