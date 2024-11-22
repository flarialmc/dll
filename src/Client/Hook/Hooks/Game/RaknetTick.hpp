#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"

class RaknetTickHook : public Hook { //RaknetConnector::tick :)

public:
    static std::string towriteip;

    static void callback(RaknetConnector *);
    static __int64 getAveragePingCallback(RakPeer*, void*);

public:
    typedef void(__thiscall *original)(RaknetConnector *);
    typedef __int64(__thiscall *originalRakPeer)(RakPeer*, void*);

    static inline original raknetTickOriginal = nullptr;
    static inline originalRakPeer getAveragePingOriginal = nullptr;

    RaknetTickHook() : Hook("RaknetTick", GET_SIG("RaknetTick")) {}

    void enableHook() override;
};