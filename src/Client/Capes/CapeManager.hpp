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

class CapeManager : public Listener
{
    std::unordered_map<std::string, Image> CapeData = {};

    void LoadCape(const std::string& CapeName, int CapeResourceID)
    {
        CapeData[CapeName] = Cape::GetCapeFromPng(CapeResourceID);
    }

public:
    void Init()
    {
        Listen(this, PacketEvent, &CapeManager::onPacket);
        Logger::debug("Init Capes");

        LoadCape("FlarialDefault", IDR_CAPE_FLARIALDEFAULT);
    }

    void onPacket(PacketEvent& event)
    {
        if (event.getPacket()->getId() == MinecraftPacketIds::PlayerSkin)
        {
            auto* Packet = reinterpret_cast<PlayerSkinPacket*>(event.getPacket());

            if (SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()[Packet->mUUID].name == SDK::clientInstance->getLocalPlayer()->getPlayerName())
            {
                Packet->mSkin.mCapeImage = CapeData["FlarialDefault"];
            }
        }
    }

    void onTick(TickEvent& event)
    {
        mcUUID uuidLocal;
        bool found = false;
        for (auto it : SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap())
        {
            if (it.second.name == SDK::clientInstance->getLocalPlayer()->getPlayerName())
            {
                found = true;
                uuidLocal = it.first;
            }

            if (!found) return;


        }
    }
};
