#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

enum class TextPacketType : int8_t {
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
    TextPacketType type;  // 0x30

    std::string name;           // 0x38
    std::string message;              // 0x58
    std::vector<std::string> mParams;   // 0x78
    std::optional<std::string> filteredMessage; // 0xA0
    bool translationNeeded = false;  // 0xB8

    std::string xuid; // 0xC0
    std::string platformId; // 0xE0

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

static_assert(sizeof(TextPacket) == 0x100);