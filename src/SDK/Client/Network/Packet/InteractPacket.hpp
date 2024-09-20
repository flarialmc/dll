#pragma once
#include <cstdint>
#include "Packet.hpp"
#include "../../../../Utils/Utils.hpp"

enum class InteractAction
{
    RIGHT_CLICK = 1,
    LEFT_CLICK = 2,
    LEAVE_VEHICLE = 3,
    MOUSEOVER = 4
};

class InteractPacket : public Packet {
public:
    enum class Action : unsigned char {
        Invalid        = 0x0,
        StopRiding     = 0x3,
        InteractUpdate = 0x4,
        NpcOpen        = 0x5,
        OpenInventory  = 0x6,
    };

public:
    InteractPacket::Action Action;
    uint64_t TargetId;
    Vec3<float> Pos;
};