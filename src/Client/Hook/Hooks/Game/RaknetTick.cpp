#include "RaknetTick.hpp"
#include "../../../../SDK/SDK.hpp"
#include "ActorBaseTick.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../../Utils/Utils.hpp"

#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string_view>

std::string RaknetTickHook::towriteip = "";

namespace {
    constexpr std::string_view kBlockedHost = "jogar.redealone.com";
    constexpr uint16_t kBlockedPort = 19132;

    std::string sanitizeHost(std::string host) {
        if (host.empty()) return host;

        host = String::toLower(std::move(host));

        // Trim whitespace characters that may appear unexpectedly
        host.erase(std::remove_if(host.begin(), host.end(), [](unsigned char c) {
            return std::isspace(c);
        }), host.end());

        return host;
    }

    bool hostMatchesBlocked(std::string host, uint16_t port) {
        if (host.empty()) return false;

        host = sanitizeHost(std::move(host));

        auto colonPos = host.find(':');
        std::string_view hostPart = colonPos == std::string::npos ? std::string_view(host)
                                                                  : std::string_view(host).substr(0, colonPos);

        if (hostPart != kBlockedHost) {
            return false;
        }

        if (port == kBlockedPort) {
            return true;
        }

        if (colonPos != std::string::npos) {
            auto portPart = host.substr(colonPos + 1);
            if (!portPart.empty()) {
                try {
                    if (std::stoi(portPart) == kBlockedPort) {
                        return true;
                    }
                } catch (...) {
                    // ignore invalid port representations
                }
            }
        }

        return false;
    }

    bool shouldBlockServer(RaknetConnector* raknet) {
        if (raknet == nullptr) {
            return false;
        }

        if (hostMatchesBlocked(raknet->JoinedIp, raknet->port)) {
            return true;
        }

        if (hostMatchesBlocked(raknet->RawIp, raknet->port)) {
            return true;
        }

        return false;
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

            std::string ip = raknet->JoinedIp;

            static bool blockTriggered = false;
            if (!blockTriggered && shouldBlockServer(raknet)) {
                blockTriggered = true;
                Logger::warn("Protected server detected ({}:{}). Disabling Flarial.",
                             raknet->JoinedIp, static_cast<int>(raknet->port));
                MessageBoxA(nullptr,
                            "Este servidor bloqueia o uso do Flarial. A client será desativada.",
                            "Flarial Bloqueado",
                            MB_OK | MB_ICONWARNING);
                Client::disable = true;
            }

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
