#include "ClientInstance.hpp"
#include "../../SDK.hpp"

LocalPlayer *ClientInstance::getLocalPlayer() {
    static int off = GET_OFFSET("ClientInstance::getLocalPlayer");
    return Memory::CallVFuncI<LocalPlayer *>(off, this);
}

BlockSource *ClientInstance::getBlockSource() {
    static int off = GET_OFFSET("ClientInstance::getBlockSource");
    return Memory::CallVFuncI<BlockSource *>(off, this);
}

void ClientInstance::grabMouse() {

    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(GET_SIG("ClientInstance::grabMouse"));
    }

    int index = *reinterpret_cast<int *>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(index, this);
}

void ClientInstance::releaseMouse() {
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(GET_SIG("ClientInstance::grabMouse"));
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
    auto address = reinterpret_cast<uintptr_t>(this);
    return *reinterpret_cast<LevelRender **>(address + GET_OFFSET("ClientInstance::levelRenderer"));
}