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
#include "../Network/Raknet/RaknetConnector.hpp"

class ClientInstance {
public:
    BUILD_ACCESS(this, MinecraftGame*, mcgame, GET_OFFSET("ClientInstance::minecraftGame"));
    BUILD_ACCESS(this, GuiData*, guiData, GET_OFFSET("ClientInstance::guiData"));
    BUILD_ACCESS(this, GLMatrix, viewMatrix, GET_OFFSET("ClientInstance::viewMatrix"));

    LocalPlayer *getLocalPlayer();

    BlockSource *getBlockSource();

    void grabMouse();

    void releaseMouse();

    static std::string getTopScreenName();

    LevelRender *getLevelRender();


    float getFovX() {
        return hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovX"));
    };

    float getFovY() {
        return hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovY"));
    };

    Vec2<float> getFov() {
        return Vec2<float>{getFovX(), getFovY()};
    };

    LoopbackPacketSender *getPacketSender() {
        return hat::member_at<LoopbackPacketSender *>(this, GET_OFFSET("ClientInstance::getPacketSender"));
    }

    RaknetConnector *getRakNetConnector() {
        if (getPacketSender() == nullptr)
            return nullptr;

        return getPacketSender()->networkSystem->remoteConnectorComposite->rakNetConnector;
    }
};
