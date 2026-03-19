#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include "ActorBaseTick.hpp"
#include <filesystem>
#include <fstream>

std::string RaknetTickHook::towriteip = "";

// SEH wrapper to safely read JoinedIp
static void tryGetJoinedIp(RaknetConnector* raknet, char* buffer, size_t bufferSize) {
    buffer[0] = '\0';
    __try {
        const auto* strPtr = &direct_access<std::string>(raknet, GET_OFFSET("RaknetConnector::JoinedIp"));
        const char* data = strPtr->c_str();
        const size_t len = strPtr->size();

        if (len < bufferSize && len < 256) {
            memcpy(buffer, data, len + 1);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        buffer[0] = '\0';
    }
}

void RaknetTickHook::callback(RaknetConnector *raknet) {
    if (getAveragePingOriginal == nullptr) {
        uintptr_t getAveragePingAddr = Memory::GetAddressByIndex(raknet->peer->vTable,


        GET_OFFSET("RakPeer::GetAveragePing"));


    }
    raknetTickOriginal(raknet);
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            char ipBuffer[256];
            tryGetJoinedIp(raknet, ipBuffer, sizeof(ipBuffer));

            std::string ip = ipBuffer;
            if (ip.empty()) {
                ip = "none";
            }

            towriteip = ip;

        }

        auto event = nes::make_holder<RaknetTickEvent>();
        eventMgr.trigger(event);
    }
}

__int64 RaknetTickHook::getAveragePingCallback(RakPeer *_this, void *guid) {
    const auto avgPing = getAveragePingOriginal(_this, guid);
    SDK::serverPing = avgPing;

    const auto getLastPingFn = reinterpret_cast<originalRakPeer>(
        Memory::GetAddressByIndex(_this->vTable, GET_OFFSET("RakPeer::GetAveragePing") + 1));
    SDK::lastPing = getLastPingFn(_this, guid);

    return avgPing;
}

void RaknetTickHook::enableHook() {
    this->autoHook((void *) callback, (void **) &raknetTickOriginal);
    this->manualHook(reinterpret_cast<void*>(GET_SIG_ADDRESS("RakPeer::GetAveragePing")), (void *) getAveragePingCallback, (void **) &getAveragePingOriginal);

}
