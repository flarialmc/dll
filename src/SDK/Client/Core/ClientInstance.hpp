#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"

#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "Minecraft.hpp"


class ClientInstance {
public:

    Minecraft* getMinecraft()
    {
        return *reinterpret_cast<Minecraft**>((uintptr_t)this + 0xD0);
    }

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);
    BUILD_ACCESS(this, GuiData*, guiData, 0x540);
    BUILD_ACCESS(this, Vec2<float>, mousePos, 0x498);

    LocalPlayer* getLocalPlayer();
    BlockSource* getBlockSource();
    void grabMouse();
    void releaseMouse();
    void refocusMouse();
    std::string getTopScreenName();

    LoopbackPacketSender* getPacketSender()
    {
        return *reinterpret_cast<LoopbackPacketSender**>((uintptr_t)this + 0xF0);
    }
};
