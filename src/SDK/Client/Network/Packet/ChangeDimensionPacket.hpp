#pragma once

#include "Packet.hpp"
#include "Utils/Utils.hpp"

class ChangeDimensionPacket : public Packet {
public:
    uint8_t mDimensionId;
    Vec3<float> mPos;
    bool respawn;
};
