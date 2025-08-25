#pragma once
#include "Events/Listener.hpp"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <ImGui/stb.h>

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/PlayerList.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"
#include "Utils/Logger/Logger.hpp"
#include "Cape.hpp"
#include "Hook/Hooks/Game/PacketHooks.hpp"

inline std::vector<std::shared_ptr<Packet> > inFlightPackets = {};

class CapeManager : public Listener
{
    std::unordered_map<std::string, Image> CapeData = {};

    void LoadCape(std::string CapeName, int CapeResourceID)
    {
        CapeData[CapeName] = Cape::GetCapeFromPng(CapeResourceID);
    }

public:
    void Init()
    {
        Listen(this, PacketEvent, &CapeManager::onPacket);
        //Listen(this, TickEvent, &CapeManager::onTick);
        Logger::debug("Init Capes");

        LoadCape("FlarialDefault", IDR_CAPE_FLARIALDEFAULT);
    }

    void onPacket(PacketEvent& event)
    {
        if (event.getPacket()->getId() == MinecraftPacketIds::PlayerSkin)
        {
            auto* Packet = reinterpret_cast<PlayerSkinPacket*>(event.getPacket());

            Logger::debug("PlayerSkin packet received");

            if (SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()[Packet->mUUID].name == SDK::clientInstance->getLocalPlayer()->getPlayerName())
            {
                Packet->mSkin.mCapeImage = Cape::GetCapeFromPng(IDR_CAPE_FLARIALDEFAULT);
                Logger::debug(Packet->mLocalizedNewSkinName);
                Logger::debug(Packet->mLocalizedOldSkinName);
            }
        }
    }

    void onTick(TickEvent& event)
    {
        mcUUID uuidLocal;
        static bool found = false;
        for (auto it : SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap())
        {
            if (it.second.name == SDK::clientInstance->getLocalPlayer()->getPlayerName())
            {
                if (found) return;
                else found = true;
                std::shared_ptr<Packet> packet = SDK::createPacket((int) MinecraftPacketIds::PlayerSkin);
                auto pSkinPacket = reinterpret_cast<PlayerSkinPacket*>(packet.get());

                pSkinPacket->mUUID = it.second.UUID;
                pSkinPacket->mSkin = it.second.playerSkin;

                if (SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback && SendPacketHook::receivePacketPlayerSkinOriginal && packet)
                    SendPacketHook::receivePacketPlayerSkinOriginal(SendPacketHook::PacketHandlerDispatcher, SendPacketHook::NetworkIdentifier, SendPacketHook::NetEventCallback, packet);

                Logger::debug("Sent skin packet");

            }
        }
    }
};
