#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include "ActorBaseTick.hpp"
#include <filesystem>
#include <fstream>
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"

std::string RaknetTickHook::towriteip = "";

void RaknetTickHook::callback(RaknetConnector *raknet) {
    if (getAveragePingOriginal == nullptr) {
        uintptr_t getAveragePingAddr = Memory::GetAddressByIndex(raknet->peer->vTable,


        GET_OFFSET("RakPeer::GetAveragePing"));


    }
    raknetTickOriginal(raknet);
    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {

            std::string ip = raknet->JoinedIp;

            if (ip.empty() && SDK::clientInstance == nullptr) {
                ip = "none";
            }

            if (ip.empty() && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() == nullptr) {
                    ip = "none";
                }
            }

            towriteip = ip;
        }

        auto event = nes::make_holder<RaknetTickEvent>();
        eventMgr.trigger(event);
    }
}

__int64 RaknetTickHook::getAveragePingCallback(RakPeer *_this, void *guid) {
    auto avgPing = getAveragePingOriginal(_this, guid);
    SDK::serverPing = avgPing;
    return avgPing;
}

void RaknetTickHook::enableHook() {
    this->autoHook((void *) callback, (void **) &raknetTickOriginal);
    this->manualHook(reinterpret_cast<void*>(GET_SIG_ADDRESS("RakPeer::GetAveragePing")), (void *) getAveragePingCallback, (void **) &getAveragePingOriginal);

}
