#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>
#include "Types/ActorRuntimeID.hpp"

class EmotePacket : public Packet {

public:
    EmotePacket();

    ActorRuntimeID runtimeId;
    std::string pieceId;
    std::string xuid;
    std::string platformId;
    int8_t flags;
};
