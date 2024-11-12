#pragma once

#include "SDK.hpp"
#include "../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

ClientInstance *SDK::clientInstance = nullptr;
ScreenView *SDK::screenView = nullptr;

std::string SDK::currentScreen;

bool SDK::hasInstanced = false;
uint64_t SDK::serverPing = 0;


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
    SDK::currentScreen = layer;
}

std::string SDK::getCurrentScreen() {
    return SDK::currentScreen;
}

int SDK::getServerPing() {
    if (!SDK::hasInstanced || !SDK::clientInstance) return -1;

    auto* localPlayer = SDK::clientInstance->getLocalPlayer();
    auto* rakNetConnector = SDK::clientInstance->getRakNetConnector();

    if (!localPlayer || !rakNetConnector) return -1;
    return rakNetConnector->JoinedIp.empty() ? 0 : static_cast<int>(SDK::serverPing);
}

std::string SDK::getServerIP() {
    if (!SDK::hasInstanced || !SDK::clientInstance) {
        return "none";
    }

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) {
        return "none";
    }

    auto raknet = SDK::clientInstance->getRakNetConnector();
    if (raknet && !raknet->JoinedIp.empty()) {
        return raknet->JoinedIp;
    }

    return player ? "world" : "none";
}
