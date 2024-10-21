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

LevelRender *ClientInstance::getLevelRender() {
    return hat::member_at<LevelRender *>(this, GET_OFFSET("ClientInstance::levelRenderer"));
}