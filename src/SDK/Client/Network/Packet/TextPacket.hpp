#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>

enum class TextPacketType {
    RAW = 0,
    CHAT = 1,
    TRANSLATION = 2,
    POPUP = 3,
    JUKEBOX_POPUP = 4,
    TIP = 5,
    SYSTEM = 6,
    WHISPER = 7,
    ANNOUNCEMENT = 8,
    JSON_WHISPER = 9,
    JSON = 10,
    JSON_ANNOUNCEMENT = 11
};

class TextPacket : public Packet {
public:
    TextPacket();

    TextPacketType type;  // 0x28

    std::string name;           // 0x30
    std::string message;              // 0x50
    std::vector<std::string> mParams;   // 0x70
    bool translationNeeded = false;  // 0x88

    std::string xuid;
    std::string platformId;
};
