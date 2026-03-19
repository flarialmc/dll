#pragma once

#include "SDK.hpp"
#include "Utils/UserActionLogger.hpp"
#include "../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include <cctype>
#include <libhat.hpp>

ClientInstance *SDK::clientInstance = nullptr;
ScreenView *SDK::screenView = nullptr;
ScreenContext* SDK::scn = nullptr;
std::vector<DrawTextQueueEntry> SDK::drawTextQueue;
std::vector<DrawTextQueueEntry> SDK::drawTextQueue2;
std::string SDK::currentScreen;

bool SDK::hasInstanced = false;
uint64_t SDK::serverPing = 0;
uint64_t SDK::lastPing = 0;

namespace {
// SEH helper to safely check if JoinedIp is valid and non-empty
bool tryGetJoinedIpSafe_SDK(RaknetConnector* raknet, char* buffer, size_t bufferSize) {
    __try {
        const auto* strPtr = &direct_access<std::string>(raknet, GET_OFFSET("RaknetConnector::JoinedIp"));
        const char* data = strPtr->c_str();
        const size_t len = strPtr->size();
        if (len == 0) {
            buffer[0] = '\0';
            return true;
        }
        if (len < bufferSize && len < 256) {
            memcpy(buffer, data, len + 1);
            return true;
        }
        buffer[0] = '\0';
        return true; // String too long, treat as empty
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        buffer[0] = '\0';
        return true; // Exception caught, treat as empty
    }
}
// Walks RemoteConnectorComposite → NetherNetConnector → mPeers to find the first
// live WebRTCNetworkPeer. Returns {peerObj, true} if found, {nullptr, false} otherwise.
// All pointer reads are inside SEH so bad offsets won't crash.
struct NetherNetPeerInfo { uint8_t* peerObj = nullptr; bool active = false; };
NetherNetPeerInfo tryGetNetherNetPeer() {
    NetherNetPeerInfo info;
    __try {
        if (!SDK::hasInstanced || !SDK::clientInstance) return info;

        auto* packetSender = SDK::clientInstance->getPacketSender();
        if (!packetSender) return info;
        auto* networkSystem = packetSender->networkSystem;
        if (!networkSystem) return info;
        auto* composite = networkSystem->remoteConnectorComposite;
        if (!composite) return info;

        // NetherNetConnector* at RemoteConnectorComposite + 0x68
        auto compositeBase = reinterpret_cast<uintptr_t>(composite);
        auto* netherNet = *reinterpret_cast<uint8_t**>(
            compositeBase + GET_OFFSET("RemoteConnectorComposite::netherNetConnector"));
        if (!netherNet) return info;

        // mPeers (std::vector<weak_ptr<WebRTCNetworkPeer>>) at NetherNetConnector + 0x1F0
        // MSVC vector layout: begin ptr (+0), end ptr (+8)
        auto peersBase = reinterpret_cast<uintptr_t>(netherNet) + GET_OFFSET("NetherNetConnector::mPeers");
        auto* begin = *reinterpret_cast<uint8_t**>(peersBase);
        auto* end   = *reinterpret_cast<uint8_t**>(peersBase + 8);

        if (!begin || begin >= end) return info; // No peers

        // First weak_ptr: object ptr (+0), control block ptr (+8)
        auto* peerObj   = *reinterpret_cast<uint8_t**>(begin);
        auto* ctrlBlock = *reinterpret_cast<uint8_t**>(begin + 8);
        if (!peerObj || !ctrlBlock) return info;

        // Verify the weak_ptr is still alive: MSVC _Ref_count_base._Uses (strong count) at +8
        auto strongCount = *reinterpret_cast<long*>(ctrlBlock + 8);
        if (strongCount <= 0) return info;

        info.peerObj = peerObj;
        info.active = true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        // Offsets may be wrong — fail silently
    }
    return info;
}

// Reads ping from a live WebRTCNetworkPeer.
// NetworkStatus at peer+0x50: mLoad(+0), mCurrentPing(+4), mAveragePing(+8)
struct NetherNetPingResult { int averagePing = -1; int currentPing = -1; };
NetherNetPingResult tryGetNetherNetPing() {
    NetherNetPingResult result;
    auto peer = tryGetNetherNetPeer();
    if (!peer.active) return result;
    __try {
        result.currentPing = *reinterpret_cast<int*>(peer.peerObj + 0x54);
        result.averagePing = *reinterpret_cast<int*>(peer.peerObj + 0x58);
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
    return result;
}
} // anonymous namespace

void SDK::pushDrawTextQueueEntry(DrawTextQueueEntry entry) {
    // auto it = std::find_if(drawTextQueue.begin(), drawTextQueue.end(), [&entry](const DrawTextQueueEntry &ent) {
    //     return entry.id == ent.id;
    // });
    // if (it != drawTextQueue.end() && std::find(drawTextQueue.begin(), drawTextQueue.end(), entry) != drawTextQueue.end()) return;
    // if (it != drawTextQueue.end()) drawTextQueue.erase(it);
    //
    drawTextQueue.push_back(std::move(entry));
}

bool SDK::containsIgnoreCase(const std::string &mainString, const std::string &searchString) {
    const auto it = std::search(
        mainString.begin(), mainString.end(),
        searchString.begin(), searchString.end(),
        [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
    );
    return it != mainString.end();
}

bool SDK::isHovered(Vec4<float> box, Vec2<float> mouse) {
    if (mouse.x >= box.x && mouse.y >= box.y && mouse.x <= box.z && mouse.y <= box.w) return true;
    return false;
}

std::shared_ptr<Packet> SDK::createPacket(int id) {
    static uintptr_t Address;

    if (Address == NULL) {
        Address = GET_SIG_ADDRESS("MinecraftPackets::createPacket");
    }

    const auto pFunction = reinterpret_cast<std::shared_ptr<Packet>(__fastcall *)(int)>(Address);
    return pFunction(id);
}

// TODO: use CI::GetScreenName
void SDK::setCurrentScreen(const std::string &layer) {
    const std::string previousScreen = currentScreen;
    currentScreen = layer;

    // Log screen change for crash telemetry
    if (previousScreen != layer) {
        UserActionLogger::logScreenChange(previousScreen, layer);
    }
}

std::string SDK::getCurrentScreen() {
    return currentScreen;
}

bgfx::Context* SDK::getBgfxContext() {
    static auto parse = hat::parse_signature(GET_SIG("bgfx::s_ctx"));
    static auto result = hat::find_pattern(parse.value(), ".text");
    return *reinterpret_cast<bgfx::Context**>(result.rel(3));
}

int SDK::getServerPing() {
    if (!hasInstanced || !clientInstance) return -1;

    const auto player = clientInstance->getLocalPlayer();
    const auto raknet = clientInstance->getRakNetConnector();

    if (!player || !raknet) return -1;

    char ipBuffer[256];
    tryGetJoinedIpSafe_SDK(raknet, ipBuffer, sizeof(ipBuffer));

    // RakNet server — use the hooked RakPeer ping
    if (ipBuffer[0] != '\0') return static_cast<int>(serverPing);

    // No RakNet IP — try NetherNet (Realm/LAN)
    auto nnPing = tryGetNetherNetPing();
    if (nnPing.averagePing >= 0) return nnPing.averagePing;

    return 0;
}

int SDK::getLastPing() {
    if (!hasInstanced || !clientInstance) return -1;

    const auto player = clientInstance->getLocalPlayer();
    const auto raknet = clientInstance->getRakNetConnector();

    if (!player || !raknet) return -1;

    char ipBuffer[256];
    tryGetJoinedIpSafe_SDK(raknet, ipBuffer, sizeof(ipBuffer));

    if (ipBuffer[0] != '\0') return static_cast<int>(lastPing);

    auto nnPing = tryGetNetherNetPing();
    if (nnPing.currentPing >= 0) return nnPing.currentPing;

    return 0;
}

std::string SDK::getServerIP() {
    if (!hasInstanced || !clientInstance) {
        return "none";
    }

    const auto player = clientInstance->getLocalPlayer();
    if (!player) {
        return "none";
    }

    // Check RakNet first — this covers traditional servers (Hive, Cubecraft, etc.)
    const auto raknet = clientInstance->getRakNetConnector();
    if (raknet) {
        char ipBuffer[256];
        tryGetJoinedIpSafe_SDK(raknet, ipBuffer, sizeof(ipBuffer));
        if (ipBuffer[0] != '\0') {
            return std::string(ipBuffer);
        }
    }

    // RakNet IP is empty but we have a player — could be singleplayer or Realm/LAN (NetherNet).
    // Check if the NetherNetConnector has active WebRTC peers. In singleplayer the connector
    // exists but mPeers is empty; on a Realm/LAN it has at least one live peer.
    if (tryGetNetherNetPeer().active) {
        return "realm";
    }

    return "world";
}

std::string SDK::getServerPort() {
    if (!hasInstanced || !clientInstance) {
        return "none";
    }

    const auto player = clientInstance->getLocalPlayer();
    if (!player) {
        return "none";
    }

    const auto raknet = clientInstance->getRakNetConnector();
    if (raknet) {
        char ipBuffer[256];
        tryGetJoinedIpSafe_SDK(raknet, ipBuffer, sizeof(ipBuffer));
        if (ipBuffer[0] != '\0') {
            return FlarialGUI::cached_to_string(raknet->port);
        }
    }

    return "0";
}
