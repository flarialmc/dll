#pragma once
#include "Events/Listener.hpp"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <ImGui/stb.h>

#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/PlayerList.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"
#include "Utils/Logger/Logger.hpp"

std::vector<unsigned char> load_png_rgba(const char* filename, int& width, int& height) {
    int n;
    unsigned char* data = stbi_load(filename, &width, &height, &n, 4); // force 4 channels: RGBA
    if (!data) {
        Logger::error("Failed to load image");
        return std::vector<unsigned char>();
    }
    std::vector<unsigned char> rgba_data(data, data + (width * height * 4));
    stbi_image_free(data);
    return rgba_data;
}

class CapeManager : public Listener
{
public:
    void Init()
    {
        Listen(this, PacketEvent, &CapeManager::onPacket);
        Logger::debug("Init Capes");
    }

    void onPacket(PacketEvent& event)
    {
        auto pkt = event.getPacket();
        if (!pkt || pkt->getId() != MinecraftPacketIds::PlayerSkin) return;
        Logger::debug("Received PlayerListPacket");

        auto* packet = reinterpret_cast<PlayerSkinPacket*>(event.getPacket());

        //if (SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap()[packet->mUUID].XUID != "2535416801160700") return;

        //Logger::debug("Player Found!");

        int mWidth, mHeight;
        auto ImageData = load_png_rgba((Utils::getAssetsPath() + "\\Cape.png").c_str(), mWidth, mHeight);

        Image Cape;

        Cape.mHeight = 32;
        Cape.mWidth = 64;
        Cape.mDepth = 0;
        Cape.mUsage = (ImageUsage)1;
        Cape.imageFormat = (ImageFormat)4;
        Cape.mImageBytes = Blob::fromVector(ImageData);

        packet->mSkin.mCapeImage = Cape;
    }
};
