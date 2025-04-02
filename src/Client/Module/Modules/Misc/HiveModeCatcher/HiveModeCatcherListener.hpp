#pragma once

#include <regex>
#include <SDK/Client/Network/Packet/ChangeDimensionPacket.hpp>
#include "../../../../Events/Network/PacketEvent.hpp"
#include "../../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"
#include "../../../../../SDK/Client/Network/Packet/TextPacket.hpp"

class HiveModeCatcherListener : public Listener {
private:
    bool connectionExecuted = false;
    bool listenForServer = false;
public:
    void onPacketReceive(PacketEvent &event)  {
        if (SDK::getServerIP().find("hive") == std::string::npos){
            HiveModeCatcherListener::fullgamemodename = "";
            HiveModeCatcherListener::currentGame = "";
            return;
        }

        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::ChangeDimension) {
            auto *pkt = reinterpret_cast<ChangeDimensionPacket *>(event.getPacket());
            if (pkt->mDimensionId == 0) {
                listenForServer = true;
                connectionExecuted = false;
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto *command_packet = reinterpret_cast<CommandRequestPacket *>(packet.get());

                command_packet->command = "/connection";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;


                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
            }
        }

        if (id == MinecraftPacketIds::Text) {
            auto *pkt = reinterpret_cast<TextPacket *>(event.getPacket());

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";


            if (pkt->message.find(textToCheck) != std::string::npos && listenForServer) {
                std::string server = pkt->message.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                HiveModeCatcherListener::currentGame = std::regex_replace(server, pattern, "");
                event.cancel();
                listenForServer = true;
                HiveModeCatcherListener::fullgamemodename = currentGame;
                //Give Hive's Gamemodes their full names. Should only be used for visual stuff.
                // HUB
                if (fullgamemodename == "HUB")
                {
                    fullgamemodename = "Hub";
                }
                // REPLAYCINEMA
                if (fullgamemodename == "REPLAY")
                {
                    fullgamemodename = "Replay Cinema";
                }
                // BEDWARS
                else if (fullgamemodename == "HUB-BED")
                {
                    fullgamemodename = "Bedwars Hub";
                }
                else if (fullgamemodename == "BED")
                {
                    fullgamemodename = "Bedwars Solo";
                }
                else if (fullgamemodename == "BED-DUOS")
                {
                    fullgamemodename = "Bedwars Duos";
                }
                else if (fullgamemodename == "BED-SQUADS")
                {
                    fullgamemodename = "Bedwars Squads";
                }
                else if (fullgamemodename == "BED-MANOR")
                {
                    fullgamemodename = "Bedwars Manor";
                }
                // SKYWARS
                else if (fullgamemodename == "HUB-SKY")
                {
                    fullgamemodename = "Skywars Hub";
                }
                else if (fullgamemodename == "SKY")
                {
                    fullgamemodename = "Skywars Solo";
                }
                else if (fullgamemodename == "SKY-DUOS")
                {
                    fullgamemodename = "Skywars Duos";
                }
                else if (fullgamemodename == "SKY-SQUADS")
                {
                    fullgamemodename = "Skywars Squads";
                }
                else if (fullgamemodename == "SKY-CLASSIC")
                {
                    fullgamemodename = "Skywars Classic Solo";
                }
                else if (fullgamemodename == "SKY-CLASSIC-SQUADS")
                {
                    fullgamemodename = "Skywars Classic Squads";
                }
                else if (fullgamemodename == "SKY-KITS")
                {
                    fullgamemodename = "Skywars Kits Solo";
                }
                else if (fullgamemodename == "SKY-KITS-DUOS")
                {
                    fullgamemodename = "Skywars Kits Duos";
                }
                // PARKOURWORLD
                else if (fullgamemodename == "HUB-PARKOUR")
                {
                    fullgamemodename = "Parkour world";
                }
                // JUSTBUILD
                else if (fullgamemodename == "BUILD")
                {
                    fullgamemodename = "Just Build Solo";
                }
                else if (fullgamemodename == "BUILD-DUOS")
                {
                    fullgamemodename = "Just Build Duos";
                }
                else if (fullgamemodename == "BUILDX")
                {
                    fullgamemodename = "Just Build Double Build Time Solo";
                }
                else if (fullgamemodename == "BUILD-DUOSX")
                {
                    fullgamemodename = "Just Build Double Build Time Duos";
                }
                // MURDER MYSTERY
                else if (fullgamemodename == "MURDER")
                {
                    fullgamemodename = "Murder Mystery";
                }
                // HIDEANDSEEK
                else if (fullgamemodename == "HIDE")
                {
                    fullgamemodename = "Hide And Seek";
                }
                // CAPTURETHEFLAG
                else if (fullgamemodename == "CTF")
                {
                    fullgamemodename = "Capture The Flag";
                }
                // DEATHRUN
                else if (fullgamemodename == "DR")
                {
                    fullgamemodename = "Death Run";
                }
                // THEBRIDGE
                else if (fullgamemodename == "BRIDGE")
                {
                    fullgamemodename = "The Bridge Solo";
                }
                else if (fullgamemodename == "BRIDGE-DUOS")
                {
                    fullgamemodename = "The Bridge Duos";
                }
                // BLOCKPARTY
                else if (fullgamemodename == "PARTY")
                {
                    fullgamemodename = "Block Party";
                }
                // SURVIVALGAME
                else if (fullgamemodename == "SG")
                {
                    fullgamemodename = "Survival Games Solo";
                }
                else if (fullgamemodename == "SG-DUOS")
                {
                    fullgamemodename = "Survival Games Duos";
                }
                else if (fullgamemodename == "SG-DUOS")
                {
                    fullgamemodename = "Survival Games Duos";
                }
                // GRAVITY
                else if (fullgamemodename == "GRAV")
                {
                    fullgamemodename = "Gravity";
                }
                // GROUNDWARS
                else if (fullgamemodename == "GROUND")
                {
                    fullgamemodename = "Ground Wars";
                }
                // BLOCKDROP
                else if (fullgamemodename == "DROP")
                {
                    fullgamemodename = "Block Drop";
                }

                //  Seasonal / custom server only games 

                // TREASUREWARS
                else if (fullgamemodename == "WARS")
                {
                    fullgamemodename = "Treasure Wars Solo";
                }
                else if (fullgamemodename == "WARS-DUOS")
                {
                    fullgamemodename = "Treasure Wars Duos";
                }
                else if (fullgamemodename == "WARS-TRIOS")
                {
                    fullgamemodename = "Treasure Wars Trios";
                }
                else if (fullgamemodename == "WARS-SQUADS")
                {
                    fullgamemodename = "Treasure Wars Squads";
                }
                else if (fullgamemodename == "WARS-MEGA")
                {
                    fullgamemodename = "Treasure Wars Mega";
                }
                // GHOSTINVASION
                else if (fullgamemodename == "GI")
                {
                    fullgamemodename = "Ghost Invasion";
                }
                // MOBGAME
                else if (fullgamemodename == "MOB")
                {
                    fullgamemodename = "Mob Game";
                }
            } else if (pkt->message.find(textToCheckToSilence) != std::string::npos) {
                event.cancel();
            }

        }
    }

    HiveModeCatcherListener() {
        Listen(this, PacketEvent, &HiveModeCatcherListener::onPacketReceive);
    }
    ~HiveModeCatcherListener() {
        Deafen(this, PacketEvent, &HiveModeCatcherListener::onPacketReceive);
    }
    static inline std::string currentGame;
    static inline std::string fullgamemodename;
};
