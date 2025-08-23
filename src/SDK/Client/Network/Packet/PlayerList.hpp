#pragma once

#include "Packet.hpp"
#include "SDK/Client/Level/Level.hpp"

class PlayerListPacket : public Packet
{
public:

    std::vector<PlayerListEntry> mEntries;
    PlayerListPacketType         mAction;
};
