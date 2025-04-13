#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/SetTitlePacket.hpp"
#include "../../../../SDK/Client/Network/Packet/ModalFormRequestPacket.hpp"
#include <lib/json/json.hpp>

class ZeqaUtils : public Module {
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
            settings.addSetting("req", true);

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


        this->addHeader("Auto re queue");
        this->addToggle("Auto re queue", "Requeue the same ranked/unranked duel after  the current one is over.", this->settings.getSettingByName<bool>("req")->value);


        this->addHeader("Auto accept");
        this->addToggle("Friend request", "Automatically accept incoming friend requests.", this->settings.getSettingByName<bool>("friendaccept")->value);
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
        std::string ip = SDK::getServerIP();
        std::transform(ip.begin(), ip.end(), ip.begin(), ::tolower);
        if (ip == "135.148.137.229" || // global, na
            ip == "141.94.99.111"   || // eu
            ip == "15.235.204.81"   || // as
            ip == "203.28.238.35"   || // au
            ip == "38.54.63.126"    || // za
            ip.find("zeqa") != std::string::npos
        ) {
            if (id == MinecraftPacketIds::Text) {
                auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

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

                        FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(61, pkt->message.length() - 68));
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

                        FlarialGUI::Notify("Accepted duel invite from: " + pkt->message.substr(23, pkt->message.length() - 48));
                    }
                }
            }
            if (id == MinecraftPacketIds::ShowModalForm) {
                if (this->settings.getSettingByName<bool>("req")->value) { //I know someone is going to copy paste this please leave some credit this took a lot of time
                    auto *pkt = reinterpret_cast<ModalFormRequestPacket *>(event.getPacket());
                    // ImGui::SetClipboardText(pkt->mFormJSON.c_str());

                    nlohmann::json jsonData = nlohmann::json::parse(pkt->mFormJSON);

                    if (jsonData.contains("buttons") && jsonData["buttons"].is_array()) {
                        for (const auto& button : jsonData["buttons"]) {
                            if (button.contains("text") && button["text"] == "\u00a7gPlay \u00a7fAgain") {
                                // Cancel ui from showing
                                event.cancel();

                                std::string content = jsonData["content"];

                                // clean the content of color codes
                                std::string cleanContent = String::removeColorCodes(content);

                                std::string gamemode = "";

                                if (cleanContent.find("Unranked") != std::string::npos) {
                                    cleanContent = cleanContent.substr(15); // Remove "Unranked"
                                    gamemode = cleanContent + " Unranked"; //add Unranked back but at the end this time
                                }
                                else if (cleanContent.find("Ranked") != std::string::npos) {
                                    cleanContent = cleanContent.substr(13); // Remove "Ranked"
                                    gamemode = cleanContent + " Ranked"; //add Ranked back but at the end this time
                                }

                                if (!gamemode.empty()) {
                                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                                    auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());
                                    command_packet->command = "/q " + gamemode;

                                    command_packet->origin.type = CommandOriginType::Player;

                                    command_packet->InternalSource = true;
                                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                                    FlarialGUI::Notify("Joined the queue for " + gamemode);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

