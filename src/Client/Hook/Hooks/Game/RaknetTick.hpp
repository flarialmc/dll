#include "../Hook.hpp"
#include "../../../../SDK/Client/Network/Raknet/RaknetConnector.hpp"

class RaknetTickHook : public Hook { //RaknetConnector::tick :)

private:
    static void callback(RaknetConnector*);

public:
    typedef void(__thiscall* original)(RaknetConnector*);

    static inline original raknetTickOriginal = nullptr;

    RaknetTickHook() : Hook("RaknetTick", "48 89 5C 24 10 48 89 74 24 18 55 57 41 54 41 56 41 57 48 8D AC 24 30 FE FF FF 48 81 EC D0 02 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 C0 01 00 00 48") {}

    void enableHook() override;
};