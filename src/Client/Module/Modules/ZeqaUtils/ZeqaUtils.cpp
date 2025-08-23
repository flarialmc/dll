#include "ZeqaUtils.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/CommandRequestPacket.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"

ZeqaUtils::ZeqaUtils(): Module("Zeqa Utils", "Handy utilities for Zeqa",
                               IDR_ZEQA_PNG, "")
{
    
}

void ZeqaUtils::onEnable()
{
    Listen(this, PacketEvent, &ZeqaUtils::onPacketReceive)
    Module::onEnable();
}

void ZeqaUtils::onDisable()
{
    Deafen(this, PacketEvent, &ZeqaUtils::onPacketReceive)
    Module::onDisable();
}

void ZeqaUtils::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("req", true);
    setDef("promomessage", false);
    setDef("join", false);
    setDef("leave", false);
    setDef("killstreak", false);
    setDef("friendaccept", false);
    setDef("duelaccept", false);
    
}

void ZeqaUtils::settingsRender(float settingsOffset)
{
    initSettingsPage();


    addHeader("Auto re queue");
    addToggle("Auto re queue", "Requeue the same ranked/unranked duel after  the current one is over.", "req");


    addHeader("Auto accept");
    addToggle("Friend request", "Automatically accept incoming friend requests.", "friendaccept");
    addToggle("duel request", "Automatically accept incoming duel requests.", "duelaccept");

    addHeader("Debloat chat");
    addToggle("Promo message", "Removes all promo/info messages", "promomessage");
    addToggle("player join", "Removes player join message", "join");
    addToggle("player leave", "Removes player leave message", "leave");
    addToggle("Kill streak", "Removes Message indicating a player has gotten a kill streak", "killstreak");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ZeqaUtils::onPacketReceive(PacketEvent& event)
{
    if (!this->isEnabled()) return;
    MinecraftPacketIds id = event.getPacket()->getId();
    std::string ip = SDK::getServerIP();
    std::transform(ip.begin(), ip.end(), ip.begin(), ::tolower);
    if (ip == "135.148.137.229" || // global, na
        ip == "141.94.99.111" || // eu
        ip == "15.235.204.81" || // as
        ip == "203.28.238.35" || // au
        ip == "38.54.63.126" || // za
        ip.find("zeqa") != std::string::npos
    ) {
        if (id == MinecraftPacketIds::Text) {
            auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());

            if (getOps<bool>("promomessage")) {
                if (pkt->message == " " ||
                    pkt->message == " " ||
                    pkt->message == " " || //onix promotion
                    pkt->message == " " ||
                    pkt->message == " ") {
                    event.cancel();
                }
            }
            if (getOps<bool>("join")) {

                if (pkt->message.substr(0, 15) == "§8[§a+§8]§a") {
                    event.cancel();
                }
            }
            if (getOps<bool>("leave")) {

                if (pkt->message.substr(0, 15) == "§8[§c-§8]§c") {
                    event.cancel();
                }
            }
            if (getOps<bool>("killstreak")) {
                if (pkt->message.contains("§g has gotten a ") && pkt->message.contains("§g killstreak")) {
                    event.cancel();
                }
            }
            if (getOps<bool>("friendaccept")) {
                if (pkt->message.find("§l§q» §r§aYou have received a friend request from ") != std::string::npos) {

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                    command_packet->command = "/f accept " + pkt->message.substr(61, pkt->message.length() - 68);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted friend invite from: " + pkt->message.substr(61, pkt->message.length() - 68));
                }
            }
            if (getOps<bool>("duelaccept")) {
                if (pkt->message.find(/*"to accept the invite!" &&*/ " §7Type §g/accept") != std::string::npos) {

                    std::shared_ptr<Packet> packet = SDK::createPacket(77);
                    auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
                    command_packet->command = "/accept " + pkt->message.substr(23, pkt->message.length() - 48);

                    command_packet->origin.type = CommandOriginType::Player;

                    command_packet->InternalSource = true;
                    SDK::clientInstance->getPacketSender()->sendToServer(command_packet);

                    FlarialGUI::Notify("Accepted duel invite from: " + pkt->message.substr(23, pkt->message.length() - 48));
                }
            }
        }
        if (id == MinecraftPacketIds::ShowModalForm) {
            if (getOps<bool>("req")) { //I know someone is going to copy paste this please leave some credit this took a lot of time
                auto* pkt = reinterpret_cast<ModalFormRequestPacket*>(event.getPacket());
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
                                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
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
