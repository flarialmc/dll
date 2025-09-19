#pragma once
#include "Packet.hpp"
class AnimatePacket : public Packet
{
public:
    // AnimatePacket inner types define
    enum class Action : int {
        NoAction         = 0,
        Swing            = 1,
        WakeUp           = 3,
        CriticalHit      = 4,
        MagicCriticalHit = 5,
    };

    uint64_t mRuntimeID;
    Action mAction;
    float mData;

};
