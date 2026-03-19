#include "GameContext.hpp"
#include "Client/Core/ClientInstance.hpp"
#include "Client/Network/Raknet/RaknetConnector.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Client/GUI/Engine/Engine.hpp"

namespace {
// SEH helper to safely check if JoinedIp is valid and non-empty
bool tryGetJoinedIpSafe_GameContext(RaknetConnector* raknet, char* buffer, size_t bufferSize) {
    __try {
        auto* strPtr = &direct_access<std::string>(raknet, GET_OFFSET("RaknetConnector::JoinedIp"));
        const char* data = strPtr->c_str();
        size_t len = strPtr->size();
        if (len == 0) {
            buffer[0] = '\0';
            return true;
        }
        if (len < bufferSize && len < 256) {
            memcpy(buffer, data, len + 1);
            return true;
        }
        buffer[0] = '\0';
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        buffer[0] = '\0';
        return true;
    }
}
} // anonymous namespace

GameContext& GameContext::instance() {
    static GameContext instance;
    return instance;
}

void GameContext::setClientInstance(ClientInstance* instance) {
    clientInstance_ = instance;
}

void GameContext::setCurrentScreen(const std::string& screen) {
    currentScreen_ = screen;
}

int GameContext::getServerPingInt() const {
    if (!hasInstanced_ || !clientInstance_) return -1;

    auto player = clientInstance_->getLocalPlayer();
    auto raknet = clientInstance_->getRakNetConnector();

    if (!player || !raknet) return -1;

    char ipBuffer[256];
    tryGetJoinedIpSafe_GameContext(raknet, ipBuffer, sizeof(ipBuffer));
    return ipBuffer[0] == '\0' ? 0 : static_cast<int>(serverPing_);
}

std::string GameContext::getServerIP() const {
    if (!hasInstanced_ || !clientInstance_) {
        return "none";
    }

    auto player = clientInstance_->getLocalPlayer();
    if (!player) {
        return "none";
    }

    auto raknet = clientInstance_->getRakNetConnector();
    if (raknet) {
        char ipBuffer[256];
        tryGetJoinedIpSafe_GameContext(raknet, ipBuffer, sizeof(ipBuffer));
        if (ipBuffer[0] != '\0') {
            return std::string(ipBuffer);
        }
    }

    return player ? "world" : "none";
}

std::string GameContext::getServerPort() const {
    if (!hasInstanced_ || !clientInstance_) {
        return "none";
    }

    auto player = clientInstance_->getLocalPlayer();
    if (!player) {
        return "none";
    }

    auto raknet = clientInstance_->getRakNetConnector();
    if (raknet) {
        char ipBuffer[256];
        tryGetJoinedIpSafe_GameContext(raknet, ipBuffer, sizeof(ipBuffer));
        if (ipBuffer[0] != '\0') {
            return FlarialGUI::cached_to_string(raknet->port);
        }
    }

    return "0";
}
