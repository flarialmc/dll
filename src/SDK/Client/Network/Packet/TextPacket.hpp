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
    BUILD_ACCESS(this, TextPacketType, type, 0x8);
    BUILD_ACCESS(this, std::string, author, 0x38);
    BUILD_ACCESS(this, std::string, message, 0x58);
    BUILD_ACCESS(this, bool, translationNeeded, 0x90);
    BUILD_ACCESS(this, std::string, xuid, 0x98);
    BUILD_ACCESS(this, std::string, platformId, 0x0B8);
};
