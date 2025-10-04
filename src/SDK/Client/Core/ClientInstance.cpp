#include "ClientInstance.hpp"
#include "../../SDK.hpp"
#include <libhat/Access.hpp>
#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "../Render/GLMatrix.hpp"
#include "../Level/LevelRender/LevelRender.hpp"
#include "../Network/Raknet/RaknetConnector.hpp"
#include "../Render/Camera.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

LocalPlayer *ClientInstance::getLocalPlayer() {
    // Indig0r
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = GET_SIG_ADDRESS("ClientInstance::getLocalPlayerIndex");
    }

    int index = *reinterpret_cast<int*>(indexRef + 9) / 8;

    return Memory::CallVFuncI<LocalPlayer *>(index, this);
}

BlockSource *ClientInstance::getBlockSource() {
    static int off = GET_OFFSET("ClientInstance::getBlockSource");
    return Memory::CallVFuncI<BlockSource *>(off, this);
}

void ClientInstance::grabMouse(int delay) {
    if (delay > 0 ) {
        std::thread troll([this, delay]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            static uintptr_t indexRef;

            if (indexRef == 0) {
                indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
            }

            int index = *reinterpret_cast<int*>(indexRef + 3) / 8;
            return Memory::CallVFuncI<void>(index, this);
        });
        troll.detach();
    } else {
        static uintptr_t indexRef;

        if (indexRef == 0) {
            indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
        }

        int index = *reinterpret_cast<int*>(indexRef + 3) / 8;
        return Memory::CallVFuncI<void>(index, this);
    }
}

void ClientInstance::releaseMouse() {
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
        if(indexRef == NULL) {
            return;
        }
    }

    int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(index + 1, this);
}

std::string ClientInstance::getTopScreenName() {
    return SDK::currentScreen;
}

std::string ClientInstance::getScreenName() {
    return SDK::currentScreen;
    // std::string screen = "no_screen";

    /*static auto sig = GET_SIG_ADDRESS("ClientInstance::getScreenName");
    auto fn = reinterpret_cast<std::string& (__thiscall *)(ClientInstance*, std::string&)>(sig);
    screen = fn(this, screen);
    return screen;*/
}


LevelRender *ClientInstance::getLevelRender() {
    return hat::member_at<LevelRender *>(this, GET_OFFSET("ClientInstance::levelRenderer"));
}

void ClientInstance::_updateScreenSizeVariables(Vec2<float> *totalScreenSize,
                                                Vec2<float> *safeZone,
                                                float forcedGuiScale) {
    static auto sig = GET_SIG_ADDRESS("ClientInstance::_updateScreenSizeVariables");
    auto fn = reinterpret_cast<void (__thiscall *)(ClientInstance*, Vec2<float>*, Vec2<float>*, float)>(sig);
    fn(this, totalScreenSize, safeZone, forcedGuiScale);
}

MinecraftGame* ClientInstance::getMinecraftGame() {
    // if (!SDK::clientInstance) return nullptr;
    return hat::member_at<MinecraftGame*>(this, GET_OFFSET("ClientInstance::minecraftGame"));
}

GuiData *ClientInstance::getGuiData() {
    return hat::member_at<GuiData*>(this, GET_OFFSET("ClientInstance::guiData"));
}

GLMatrix ClientInstance::getViewMatrix() {
    return hat::member_at<GLMatrix>(this, GET_OFFSET("ClientInstance::viewMatrix"));
}

mce::Camera& ClientInstance::getCamera() {
    static int off = GET_OFFSET("ClientInstance::camera");
    return hat::member_at<mce::Camera>(this, off);
}

float ClientInstance::getFovX() {
    if (!VersionUtils::checkAboveOrEqual(21, 110)) return hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovX"));

    if (this->getLevelRender()) if (this->getLevelRender()->getLevelRendererPlayer())
    return this->getLevelRender()->getLevelRendererPlayer()->getFovX();

    return 0.0f;
}

float ClientInstance::getFovY() {
    if (!VersionUtils::checkAboveOrEqual(21, 110)) return hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovY"));

    if (this->getLevelRender()) if (this->getLevelRender()->getLevelRendererPlayer())
    return this->getLevelRender()->getLevelRendererPlayer()->getFovY();

    return 0.0f;
}

Vec2<float> ClientInstance::getFov() {
    return Vec2<float>{getFovX(), getFovY()};
}

LoopbackPacketSender *ClientInstance::getPacketSender() {
    return hat::member_at<LoopbackPacketSender *>(this, GET_OFFSET("ClientInstance::getPacketSender"));
}

RaknetConnector *ClientInstance::getRakNetConnector() {
    if (getPacketSender() == nullptr)
        return nullptr;

    return getPacketSender()->networkSystem->remoteConnectorComposite->rakNetConnector;
}