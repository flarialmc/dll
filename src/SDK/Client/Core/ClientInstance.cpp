#include "ClientInstance.hpp"
#include "../../SDK.hpp"

LocalPlayer* ClientInstance::getLocalPlayer() {
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(
            "49 8B 00 49 8B C8 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 0F 57 C0 0F 11 85 ? ? ? ? 66 0F 7F 44 24 ? 48 8B 05 ? ? ? ? 48 85 C0 74 04");
    }

    int index = *reinterpret_cast<int*>(indexRef + 9) / 8;
    return Memory::CallVFuncI<LocalPlayer*>(this, index);
}

BlockSource* ClientInstance::getBlockSource() {
    return Memory::CallVFunc<26, BlockSource*>(this);
}
void ClientInstance::grabMouse() {

    static uintptr_t indexRef;
  
    if (indexRef == 0) {
        indexRef = Memory::findSig(
            "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 90 48 85 DB 74 08 48 8B CB E8 ? ? ? ? 48 8B 8F ? ? ? ? E8 ? ? ? ? 33 D2");
    }
    
    int index = *reinterpret_cast<int*>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(this, index);
}

void ClientInstance::releaseMouse() {
    static uintptr_t indexRef;

    if (indexRef == 0) {
        indexRef = Memory::findSig(
            "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 90 48 85 DB 74 08 48 8B CB E8 ? ? ? ? 48 8B 8F ? ? ? ? E8 ? ? ? ? 33 D2");
    }

    int index = *reinterpret_cast<int*>(indexRef + 3) / 8;
    return Memory::CallVFuncI<void>(this, index + 1);
}

void ClientInstance::refocusMouse() {
    return Memory::CallVFunc<307, void>(this);
}
std::string ClientInstance::getTopScreenName() {
    return SDK::CurrentScreen;
}

