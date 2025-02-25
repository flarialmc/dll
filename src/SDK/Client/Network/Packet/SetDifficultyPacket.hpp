#pragma once

#include "Packet.hpp"

class SetDifficultyPacket : public Packet {
    uint8_t difficulty;
};