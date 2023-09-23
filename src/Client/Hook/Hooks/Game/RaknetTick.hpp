#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"

class RaknetTickHook : public Hook { //RaknetConnector::tick :)

private:
    static void callback(RaknetConnector*);

public:
    static std::string towriteip;

    typedef void(__thiscall* original)(RaknetConnector*);

    static inline original raknetTickOriginal = nullptr;

    RaknetTickHook() : Hook("RaknetTick", "48 8B 81 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B C4") {}

    void enableHook() override;
};