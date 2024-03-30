#include "ClientInstance.hpp"
#include "../../SDK.hpp"

LocalPlayer *ClientInstance::getLocalPlayer() {
    return Memory::CallVFunc<29, LocalPlayer *>(this);
}

BlockSource *ClientInstance::getBlockSource() {
    return Memory::CallVFunc<28, BlockSource *>(this);
}

void ClientInstance::grabMouse() {

    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(GET_SIG("ClientInstance::grabMouse"));
    }

    int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(this, index);
}

void ClientInstance::releaseMouse() {
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(GET_SIG("ClientInstance::grabMouse"));
    }

    int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(this, index + 1);
}

void ClientInstance::refocusMouse() {
    return Memory::CallVFunc<333, void>(this);
}

std::string ClientInstance::getTopScreenName() {
    return SDK::currentScreen;
}

LevelRender *ClientInstance::getLevelRender() {
    auto address = reinterpret_cast<uintptr_t>(this);
    return *reinterpret_cast<LevelRender **>(address + 0xE0);
}