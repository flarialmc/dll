#include "ClientInstance.hpp"
#include "../../SDK.hpp"
#include <libhat/Access.hpp>

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

void ClientInstance::grabMouse() {

    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = GET_SIG_ADDRESS("ClientInstance::grabMouse");
    }

    int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(index, this);
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
    std::string screen = "no_screen";

    static auto sig = GET_SIG_ADDRESS("ClientInstance::getScreenName");
    auto fn = reinterpret_cast<std::string& (__thiscall *)(ClientInstance*, std::string&)>(sig);
    screen = fn(this, screen);
    return screen;
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