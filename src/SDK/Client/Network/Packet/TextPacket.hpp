#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>

enum class TextPacketType : int {
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
    TextPacketType type;  // 0x28

    std::string name;           // 0x30
    std::string message;              // 0x50
    std::vector<std::string> mParams;   // 0x70
    bool translationNeeded = false;  // 0x88

    std::string xuid;
    std::string platformId;

    TextPacket() = default;
    //code has peaked
    //AKA this is the simplest  way to do it
    static std::string typeToString(TextPacketType type) {
        switch (type) {
            case TextPacketType::RAW: return "RAW";
            case TextPacketType::CHAT: return "CHAT";
            case TextPacketType::TRANSLATION: return "TRANSLATION";
            case TextPacketType::POPUP: return "POPUP";
            case TextPacketType::JUKEBOX_POPUP: return "JUKEBOX_POPUP";
            case TextPacketType::TIP: return "TIP";
            case TextPacketType::SYSTEM: return "SYSTEM";
            case TextPacketType::WHISPER: return "WHISPER";
            case TextPacketType::JSON_WHISPER: return "JSON_WHISPER";
            case TextPacketType::JSON: return "JSON";
            case TextPacketType::JSON_ANNOUNCEMENT: return "JSON_ANNOUNCEMENT";
            default:
                return "UNKNOWN";

        }
    }
};
