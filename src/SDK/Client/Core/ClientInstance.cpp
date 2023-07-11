#include "ClientInstance.hpp"
#include "../../SDK.hpp"

LocalPlayer* ClientInstance::getLocalPlayer() {
	return Memory::CallVFunc<27, LocalPlayer*>(this);
}

BlockSource* ClientInstance::getBlockSource() {
    return Memory::CallVFunc<26, BlockSource*>(this);
}
void ClientInstance::grabMouse() {
    return Memory::CallVFunc<306, void>(this);
}

void ClientInstance::releaseMouse() {
    return Memory::CallVFunc<307, void>(this);
}

void ClientInstance::refocusMouse() {
    return Memory::CallVFunc<307, void>(this);
}
std::string ClientInstance::getTopScreenName() {
    return SDK::CurrentScreen;
}

