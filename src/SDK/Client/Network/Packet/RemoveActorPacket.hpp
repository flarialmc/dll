#pragma once

#include <cstdint>
#include "Packet.hpp"

class RemoveActorPacket : public Packet {
public:
    int64_t mActorUniqueID;

    RemoveActorPacket() = default;
};
