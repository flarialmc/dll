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


    Minecraft *getMinecraft() {
        return *reinterpret_cast<Minecraft **>((uintptr_t) this + 0xD0);
    }

    BUILD_ACCESS(this, MinecraftGame*, mcgame, GET_OFFSET("ClientInstance::minecraftGame"));
    BUILD_ACCESS(this, GuiData*, guiData, GET_OFFSET("ClientInstance::guiData"));
    BUILD_ACCESS(this, Camera, camera, GET_OFFSET("ClientInstance::camera"));
    BUILD_ACCESS(this, GLMatrix, Matrix1, GET_OFFSET("ClientInstance::Matrix1"));
    BUILD_ACCESS(this, ClientHMDState, HMDState, GET_OFFSET("ClientInstance::clientHMDState"));

    LocalPlayer *getLocalPlayer();

    BlockSource *getBlockSource();

    void grabMouse();

    void releaseMouse();

    void refocusMouse();

    static std::string getTopScreenName();

    LevelRender *getLevelRender();


    float *getFovX() {
        return reinterpret_cast<float *>((uintptr_t) (this) + GET_OFFSET("ClientInstance::getFovX"));
    };

    float *getFovY() {
        return reinterpret_cast<float *>((uintptr_t) (this) + GET_OFFSET("ClientInstance::getFovY"));
    };

    Vec2<float> getFov() {
        return Vec2<float>{*getFovX(), *getFovY()};
    };

    LoopbackPacketSender *getPacketSender() {
        return *reinterpret_cast<LoopbackPacketSender **>((uintptr_t) this + GET_OFFSET("ClientInstance::getPacketSender"));
    }

    RaknetConnector *getRakNetConnector() {
        if (getPacketSender() == nullptr)
            return nullptr;

        return getPacketSender()->networkSystem->remoteConnectorComposite->rakNetConnector;
    }

    Matrix const &getProjectionMatrix() {
        return this->HMDState.lastLevelProjMatrix;
    }
};
