#pragma once

#include "Packet.hpp"
#include "Types/ContainerID.hpp"
#include "../../Level/HitResult.hpp"
#include "Types/ContainerType.hpp"

class ContainerOpenPacket : public Packet {
public:
    ContainerID ContainerId;
    ContainerType Type;
    BlockPos Pos;
    int64_t EntityUniqueID;
};