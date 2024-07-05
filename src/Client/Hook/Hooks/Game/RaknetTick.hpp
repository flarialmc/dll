#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"

class RaknetTickHook : public Hook { //RaknetConnector::tick :)

private:
    static void callback(RaknetConnector *);

public:
    static std::string towriteip;

    typedef void(__thiscall *original)(RaknetConnector *);

    static inline original raknetTickOriginal = nullptr;

    RaknetTickHook() : Hook("RaknetTick",GET_SIG("RaknetTick")) {}

    void enableHook() override;
};