#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"
#include "json/json.h"

class ParseThirdPartyServersHook : public Hook { 

public:
    static bool callback(void* a1,
        Json::Value* resultsArray,
        int count,
        void* backend,
        int32_t searchType);
public:
    static inline bool(*original)(void* a1, Json::Value* resultsArray,  int count, void* backend, int32_t searchType);

    ParseThirdPartyServersHook() : Hook("ParseThirdPartyServers", GET_SIG_ADDRESS("ParseThirdPartyServers")) {}
    void enableHook() override;
};