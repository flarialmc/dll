#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"


class ZeqaUtils : public Module {
private:
    std::string gamemode;
    bool ranked = false;
public:
    ZeqaUtils() : Module("Zeqa Utils", "Handy utilities for Zeqa",
                      IDR_ZEQA_PNG, "") {
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

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<bool>("req") == nullptr)
            settings.addSetting("req", false);
        if (settings.getSettingByName<std::string>("gamemode") == nullptr)
            settings.addSetting("gamemode", (std::string)"FireballMace");
        if (settings.getSettingByName<std::string>("rank") == nullptr)
            settings.addSetting("rank", (std::string)"unranked");

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
        if (settings.getSettingByName<bool>("duelaccept") == nullptr)
            settings.addSetting("duelaccept", false);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Auto re queue"); //TODO: cancel ui packet with playagain, request remach etc
        this->addToggle("Auto re queue", "Queue selected duel when the current one is over.", this->settings.getSettingByName<bool>("req")->value);
        this->addDropdown("Gamemode", "Gamemode to re-q  to",  std::vector<std::string>{
            "FireballMace",
           "CrystalPvP",
           "TopFight",
           "Parkour",
           "MidFight",
           "Invaded",
           "Boxing",
           "FireballFight",
           "BedFight",
           "Nodebuff",
           "Bridge",
           "BattleRush",
           "Sumo",
           "Combo",
           "BuildUHC",
           "Classic",
           "FinalUHC",
           "MLGRush",
           "Archer",
           "CaveUHC",
           "NetherUHC",
           "Skywars",
           "Spleef",
           "PearlFight",
           "Fist",
           "Gapple",
           "StickFight",
           "Soup",
           "SnowballSumo",
           "Debuff",
           "Wrath",
           "SG"
        }, this->settings.getSettingByName<std::string>("gamemode")->value);

        this->addDropdown("(un)ranked","Re-q for ranked or unranked",  std::vector<std::string>{
            "ranked",
            "unranked"
        },this->settings.getSettingByName<std::string>("rank")->value);

        this->addHeader("Auto accept");
        this->addToggle("Friend request", "Automatically accept incoming friend requests.", this->settings.getSettingByName<bool>("friendaccept")->value);
        // this->addToggle("Party request", "Automatically accept incoming party requests.", this->settings.getSettingByName<bool>("partyaccept")->value); //ok so an accept party command doesn't exist?
        this->addToggle("duel request", "Automatically accept incoming duel requests.", this->settings.getSettingByName<bool>("duelaccept")->value);

        this->addHeader("Debloat chat");
        this->addToggle("Promo message", "Removes all promo/info messages", this->settings.getSettingByName<bool>("promomessage")->value);
        this->addToggle("player join", "Removes player join message", this->settings.getSettingByName<bool>("join")->value);
        this->addToggle("player leave", "Removes player leave message", this->settings.getSettingByName<bool>("leave")->value);
        this->addToggle("Kill streak", "Removes Message indicating a player has gotten a kill streak", this->settings.getSettingByName<bool>("killstreak")->value);



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
                if (pkt->message == " " ||
                    pkt->message == " " ||
                    pkt->message == " " || //onix promotion
                    pkt->message == " " ||
                    pkt->message == " "){
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

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                    command_packet->command = "/f accept " + pkt->message.substr(61, pkt->message.length() - 68);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(61, pkt->message.length() - 68));  //untested
                }
            }
            if (this->settings.getSettingByName<bool>("duelaccept")->value) {
                if (pkt->message.find(/*"to accept the invite!" &&*/ " §7Type §g/accept")!= std::string::npos){

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                    command_packet->command = "/accept " + pkt->message.substr(23, pkt->message.length() - 48);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted duel invite from: " + pkt->message.substr(23, pkt->message.length() - 48)); //untested
                }
            }
        }
        if (id == MinecraftPacketIds::SetTitle){
            auto *pkt = reinterpret_cast<SetTitlePacket *>(event.getPacket());
            if (this->settings.getSettingByName<bool>("req")->value){
                if (pkt->text == "§f§aYou won the game!" || //pre s8
                    pkt->text == "§f§cYou lost the game!" || //pre s8
                    pkt->text == "   " || //s8
                    pkt->text == "   "){ //s8
                    std::thread t([this]() { //make a thread to not make whole mc sleep
                        std::this_thread::sleep_for(std::chrono::milliseconds(2500)); //wait for the player to be teleported to the hub where they can execute queue command (timing not perfect yet Can anyone help test?)

                        std::shared_ptr<Packet> packet = SDK::createPacket(77);
                        auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                        command_packet->command = "/q " + this->settings.getSettingByName<std::string>("gamemode")->value + " " + this->settings.getSettingByName<std::string>("rank")->value;

                        command_packet->origin.type = CommandOriginType::Player;

                        command_packet->InternalSource = true;
                        SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                        FlarialGUI::Notify("Joined the queue for " + this->settings.getSettingByName<std::string>("gamemode")->value + " " + this->settings.getSettingByName<std::string>("rank")->value);
                    });
                    t.detach();
                }
            }
        }
    }
};

