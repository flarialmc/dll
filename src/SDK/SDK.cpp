#pragma once

#include "SDK.hpp"
#include "../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

ClientInstance *SDK::clientInstance = nullptr;
ScreenView *SDK::screenView = nullptr;
std::string SDK::currentScreen;
bool SDK::hasInstanced = false;
std::chrono::steady_clock::time_point SDK::lastSetCurrentScreenTime;

void SDK::setCI() {
    struct bruh {
        struct bruh2 {
            struct bruh3 {
                char pad[0x58];
                struct bruh4 {
                    ClientInstance *ci;
                } *bruh4;
            } *bruh3;
        } *bruh2;
    };

    static bruh *off;

    if (off == nullptr) {
        uintptr_t addr = Memory::findSig(GET_SIG("clientInstanceSig"));
        off = reinterpret_cast<bruh *>(Memory::offsetFromSig(addr, 3));
    }

    clientInstance = off->bruh2->bruh3->bruh4->ci;
}

std::shared_ptr<Packet> SDK::createPacket(int id) {

    static uintptr_t Address;

    if (Address == NULL) {
        Address = Memory::findSig(GET_SIG("MinecraftPackets::createPacket"));
    }

    auto pFunction = reinterpret_cast<std::shared_ptr<Packet>(__fastcall *)(int)>(Address);
    return pFunction(id);
}

void SDK::setCurrentScreen(const std::string& layer) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastSetCurrentScreenTime).count();

    if (layer != SDK::currentScreen && elapsedTime >= (layer == SDK::currentScreen ? 16 : 500)) {
        lastSetCurrentScreenTime = std::chrono::steady_clock::now();
        SDK::currentScreen = layer;
    }

    SDK::lastSetCurrentScreenTime = std::chrono::steady_clock::now();
    SDK::currentScreen = layer;
}

int SDK::getServerPing() {
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            if (SDK::clientInstance->getRakNetConnector() != nullptr) {
                if (SDK::clientInstance->getRakNetConnector()->JoinedIp.empty()) {
                    return 0;
                } else {
                    return SDK::clientInstance->getRakNetConnector()->peer->getPing();
                }
            }
        }
    }
    return 1;
}

std::string SDK::getServerIP() {
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            std::string ip{};
            if (SDK::clientInstance->getRakNetConnector() != nullptr) {
                ip = SDK::clientInstance->getRakNetConnector()->JoinedIp;
                if (!ip.empty()) {
                    return ip;
                }
            }
        }
    }
    return "world";
}
