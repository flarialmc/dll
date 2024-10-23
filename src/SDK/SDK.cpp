#pragma once

#include "SDK.hpp"
#include "../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

ClientInstance *SDK::clientInstance = nullptr;
ScreenView *SDK::screenView = nullptr;

std::mutex SDK::currentScreenMtx;
std::string SDK::currentScreen;

bool SDK::hasInstanced = false;
uint64_t SDK::serverPing = 0;


std::chrono::steady_clock::time_point SDK::lastSetCurrentScreenTime;

std::shared_ptr<Packet> SDK::createPacket(int id) {

    static uintptr_t Address;

    if (Address == NULL) {
        Address = GET_SIG_ADDRESS("MinecraftPackets::createPacket");
    }

    auto pFunction = reinterpret_cast<std::shared_ptr<Packet>(__fastcall *)(int)>(Address);
    return pFunction(id);
}

// TODO: use CI::GetScreenName
void SDK::setCurrentScreen(const std::string& layer) {
    std::lock_guard<std::mutex> guard(currentScreenMtx);
    SDK::currentScreen = layer;
}

std::string SDK::getCurrentScreen() {
    std::lock_guard<std::mutex> guard(currentScreenMtx);
    return SDK::currentScreen;
}

int SDK::getServerPing() {
    if(WinrtUtils::check(21, 40)) return -1;
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            if (SDK::clientInstance->getRakNetConnector() != nullptr) {
                if (SDK::clientInstance->getRakNetConnector()->JoinedIp.empty()) {
                    return 0;
                } else {
                    return (int)SDK::serverPing;
                }
            }
        }
    }
    return -1;
}

std::string SDK::getServerIP() {
    if(WinrtUtils::check(21, 40)) return "";
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            std::string ip{};
            if (SDK::clientInstance->getRakNetConnector() != nullptr) {
                ip = SDK::clientInstance->getRakNetConnector()->JoinedIp;
                if (!ip.empty()) {
                    return ip;
                } else{
                    return "world";
                }
            }
        }
    }
    return "none";
}
