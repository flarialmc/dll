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
#include "Camera.hpp"
#include "ClientHMDState.hpp"
#include "../Network/Raknet/RaknetConnector.hpp"

class ClientInstance {
public:



    Minecraft* getMinecraft()
    {
        return *reinterpret_cast<Minecraft**>((uintptr_t)this + 0xD0);
    }

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);
    BUILD_ACCESS(this, GuiData*, guiData, 0x558);
    BUILD_ACCESS(this, Camera, camera, 0x270);
    BUILD_ACCESS(this, GLMatrix, Matrix1, 0x0330);
    BUILD_ACCESS(this, ClientHMDState, HMDState, 0x5A8);

    LocalPlayer* getLocalPlayer();
    BlockSource* getBlockSource();
    void grabMouse();
    void releaseMouse();
    void refocusMouse();
    std::string getTopScreenName();

	LevelRender* getLevelRender();


	float* getFovX()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6F0);
	};

	float* getFovY()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x704);
	};

	Vec2<float> getFov()
	{
		return Vec2(*getFovX(), *getFovY());
	};

    LoopbackPacketSender* getPacketSender()
    {
        return *reinterpret_cast<LoopbackPacketSender**>((uintptr_t)this + 0xF0);
    }

	RaknetConnector* getRakNetConnector() {
	    if (getPacketSender() == nullptr)
	    	return nullptr;

    	return getPacketSender()->networkSystem->remoteConnectorComposite->rakNetConnector;
    }

    Matrix const& getProjectionMatrix() {
        return this->HMDState.lastLevelProjMatrix;
    }
};
