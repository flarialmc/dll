#pragma once

#include <regex>
#include <SDK/Client/Network/Packet/ChangeDimensionPacket.hpp>
#include "../../../../Events/Network/PacketEvent.hpp"
#include "../../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"
#include "../../../../../SDK/Client/Network/Packet/TextPacket.hpp"
#include "../../Client/Events/Listener.hpp"
#include "../../Client/Events/Network/PacketEvent.hpp"


class HiveModeCatcherListener : public Listener
{
private:
    bool connectionExecuted = false;
    bool listenForServer = false;

public:
    void onPacketReceive(PacketEvent& event)
    {
        if (SDK::getServerIP().find("hive") == std::string::npos)
        {
            HiveModeCatcherListener::fullgamemodename = "";
            HiveModeCatcherListener::currentGame = "";
            return;
        }

        MinecraftPacketIds id = event.getPacket()->getId();

        if (id == MinecraftPacketIds::ChangeDimension)
        {
            auto* pkt = reinterpret_cast<ChangeDimensionPacket*>(event.getPacket());
            if (pkt->mDimensionId == 0)
            {
                listenForServer = true;
                connectionExecuted = false;
                std::shared_ptr<Packet> packet = SDK::createPacket(77);
                auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());

                command_packet->command = "/connection";

                command_packet->origin.type = CommandOriginType::Player;

                command_packet->InternalSource = true;


                SDK::clientInstance->getPacketSender()->sendToServer(command_packet);
            }
        }

        if (id == MinecraftPacketIds::Text)
        {
            auto pktOpt = getTextPacket(event.getPacket());
            if (!pktOpt) return;
            auto& pkt = *pktOpt;

            std::string textToCheck = "You are connected to server name ";
            std::string textToCheckToSilence = "You are connected";

            if (pkt.message.find(textToCheck) != std::string::npos && listenForServer)
            {
                std::string newMessage(pkt.message);
                std::string server = newMessage.replace(0, textToCheck.length(), "");
                std::regex pattern("\\d+");
                HiveModeCatcherListener::currentGame = std::regex_replace(server, pattern, "");
                event.cancel();
                listenForServer = true;
                HiveModeCatcherListener::fullgamemodename = currentGame;
                //Give Hive's Gamemodes their full names. Should only be used for visual stuff.
                // HUB
                if (fullgamemodename == "HUB")
                {
                    fullgamemodename = "The Hub";
                }
                // REPLAYCINEMA
                if (fullgamemodename == "REPLAY")
                {
                    fullgamemodename = "Replay Cinema";
                }
                // BEDWARS
                else if (fullgamemodename == "HUB-BED")
                {
                    fullgamemodename = "BedWars Hub";
                }
                else if (fullgamemodename == "BED")
                {
                    fullgamemodename = "BedWars Solos";
                }
                else if (fullgamemodename == "BED-DUOS")
                {
                    fullgamemodename = "BedWars Duos";
                }
                else if (fullgamemodename == "BED-SQUADS")
                {
                    fullgamemodename = "BedWars Squads";
                }
                else if (fullgamemodename == "BED-MANOR")
                {
                    fullgamemodename = "BedWars Manor";
                }
                else if (fullgamemodename == "BED-MEGA")
                {
                    fullgamemodename = "BedWars Mega";
                }
                // SKYWARS
                else if (fullgamemodename == "HUB-SKY")
                {
                    fullgamemodename = "SkyWars Hub";
                }
                else if (fullgamemodename == "SKY")
                {
                    fullgamemodename = "SkyWars Solos";
                }
                else if (fullgamemodename == "SKY-DUOS")
                {
                    fullgamemodename = "SkyWars Duos";
                }
                else if (fullgamemodename == "SKY-SQUADS")
                {
                    fullgamemodename = "SkyWars Squads";
                }
                else if (fullgamemodename == "SKY-CLASSIC")
                {
                    fullgamemodename = "SkyWars Classic Solos";
                }
                else if (fullgamemodename == "SKY-CLASSIC-SQUADS")
                {
                    fullgamemodename = "SkyWars Classic Squads";
                }
                else if (fullgamemodename == "SKY-KITS")
                {
                    fullgamemodename = "SkyWars Kits Solos";
                }
                else if (fullgamemodename == "SKY-KITS-DUOS")
                {
                    fullgamemodename = "SkyWars Kits Duos";
                }
                // PARKOURWORLD
                else if (fullgamemodename == "HUB-PARKOUR")
                {
                    fullgamemodename = "Parkour World";
                }
                // BUILDBATTLE
                else if (fullgamemodename == "HUB-BUILD")
                {
                    fullgamemodename = "Build Battle Hub";
                }
                else if (fullgamemodename == "BUILD")
                {
                    fullgamemodename = "Build Battle Solos";
                }
                else if (fullgamemodename == "BUILD-DUOS")
                {
                    fullgamemodename = "Build Battle Duos";
                }
                else if (fullgamemodename == "BUILDX")
                {
                    fullgamemodename = "Build Battle Solos Extended";
                }
                else if (fullgamemodename == "BUILD-DUOSX")
                {
                    fullgamemodename = "Build Battle Duos Extended";
                }
                else if (fullgamemodename == "BUILD-SPEED")
                {
                    fullgamemodename = "Speed Builders";
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
                    fullgamemodename = "Survival Games Solos";
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
                    fullgamemodename = "Treasure Wars Solos";
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
            }
            else if (pkt.message.find(textToCheckToSilence) != std::string::npos)
            {
                event.cancel();
            }
        }
    }

    HiveModeCatcherListener()
    {
        Listen(this, PacketEvent, &HiveModeCatcherListener::onPacketReceive);
    }

    ~HiveModeCatcherListener()
    {
        Deafen(this, PacketEvent, &HiveModeCatcherListener::onPacketReceive);
    }

    static inline std::string currentGame;
    static inline std::string fullgamemodename;
};