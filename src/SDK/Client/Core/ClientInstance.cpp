#include "ClientInstance.hpp"
#include "../../SDK.hpp"

LocalPlayer* ClientInstance::getLocalPlayer() {
    return Memory::CallVFunc<29,LocalPlayer*>(this);
}

BlockSource* ClientInstance::getBlockSource() {
    return Memory::CallVFunc<28, BlockSource*>(this);
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
    return Memory::CallVFunc<333, void>(this);
}
std::string ClientInstance::getTopScreenName() {
    return SDK::CurrentScreen;
}

LevelRender* ClientInstance::getLevelRender()
{
    uintptr_t address = reinterpret_cast<uintptr_t>(this);
    return *reinterpret_cast<LevelRender**>(address + 0xE0);
}