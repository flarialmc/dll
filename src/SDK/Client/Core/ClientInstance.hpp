#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"
#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "Minecraft.hpp"
#include "../Render/GLMatrix.hpp"
#include "../Level/LevelRender/LevelRender.hpp"


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

	LevelRender* getLevelRender();


	float* getFovX()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6F8);
	};

	float* getFovY()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x70C);
	};

	Vec2<float> getFov()
	{
		return Vec2(*getFovX(), *getFovY());
	};

    LoopbackPacketSender* getPacketSender()
    {
        return *reinterpret_cast<LoopbackPacketSender**>((uintptr_t)this + 0xF0);
    }
};
