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

    RaknetTickHook() : Hook("RaknetTick", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 45 33 FF 4C 89 BD ? ? ? ? 48 8D 95 ? ? ? ?") {}

    void enableHook() override;
};