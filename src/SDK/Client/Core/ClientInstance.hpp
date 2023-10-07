#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"
#include "LevelRender.hpp"
#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "Minecraft.hpp"
#include "../Render/GLMatrix.hpp"


class ClientInstance {
public:



    Minecraft* getMinecraft()
    {
        return *reinterpret_cast<Minecraft**>((uintptr_t)this + 0xD0);
    }

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);
    BUILD_ACCESS(this, GuiData*, guiData, 0x560);
    BUILD_ACCESS(this, GLMatrix, Matrix1, 0x0330);

    LocalPlayer* getLocalPlayer();
    BlockSource* getBlockSource();
    void grabMouse();
    void releaseMouse();
    void refocusMouse();
    std::string getTopScreenName();

	LevelRender* getLevelRender()
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(this);
		return *reinterpret_cast<LevelRender**>(address + 0xE0);
	}

    /*
	float* getFovX()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6A0);
	};

	float* getFovY()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6B4);
	};

	Vec2<float> getFov()
	{
		return Vec2(*getFovX(), *getFovY());
	};*/

    LoopbackPacketSender* getPacketSender()
    {
        return *reinterpret_cast<LoopbackPacketSender**>((uintptr_t)this + 0xF0);
    }
};
